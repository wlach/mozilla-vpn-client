#!/bin/bash
# SPDX-License-Identifier: GPL-2.0
#
# Copyright (C) 2015-2020 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
#

echo "Running mozillavpn forked copy of wg-quick script."

set -e -o pipefail
shopt -s extglob
export LC_ALL=C

INTERFACE="moz0"

die() {
	echo "MozillaVPN WGQuick Fatal Error: $*" >&2
	exit 1
}

del_if() {
	local table
	[[ $HAVE_SET_FIREWALL -eq 0 ]] || remove_firewall
	if get_fwmark table && [[ $(wg show "$INTERFACE" allowed-ips) =~ /0(\ |$'\n'|$) ]]; then
		while [[ $(ip -4 rule show 2>/dev/null) == *"lookup $table"* ]]; do
			ip -4 rule delete table $table
		done
		while [[ $(ip -4 rule show 2>/dev/null) == *"from all lookup main suppress_prefixlength 0"* ]]; do
			ip -4 rule delete table main suppress_prefixlength 0
		done
		while [[ $(ip -6 rule show 2>/dev/null) == *"lookup $table"* ]]; do
			ip -6 rule delete table $table
		done
		while [[ $(ip -6 rule show 2>/dev/null) == *"from all lookup main suppress_prefixlength 0"* ]]; do
			ip -6 rule delete table main suppress_prefixlength 0
		done
	fi
}

add_route() {
	local proto=-4
	[[ $1 == *:* ]] && proto=-6
	[[ $TABLE != off ]] || return 0

	if [[ -n $TABLE && $TABLE != auto ]]; then
		cmd ip $proto route add "$1" dev "$INTERFACE" table "$TABLE"
	elif [[ $1 == */0 ]]; then
		add_default "$1"
	else
		[[ -n $(ip $proto route show dev "$INTERFACE" match "$1" 2>/dev/null) ]] || cmd ip $proto route add "$1" dev "$INTERFACE"
	fi
}

get_fwmark() {
	local fwmark
	fwmark="$(wg show "$INTERFACE" fwmark)" || return 1
	[[ -n $fwmark && $fwmark != off ]] || return 1
	printf -v "$1" "%d" "$fwmark"
	return 0
}

remove_firewall() {
	local line iptables found restore
	for iptables in iptables ip6tables; do
		restore="" found=0
		while read -r line; do
			[[ $line == "*"* || $line == COMMIT || $line == "-A "*"-m comment --comment \"wg-quick(8) rule for $INTERFACE\""* ]] || continue
			[[ $line == "-A"* ]] && found=1
			printf -v restore '%s%s\n' "$restore" "${line/#-A/-D}"
		done < <($iptables-save 2>/dev/null)
		[[ $found -ne 1 ]] || echo -n "$restore" | $iptables-restore -n
	done
}

HAVE_SET_FIREWALL=0
add_default() {
	local table line
	if ! get_fwmark table; then
		table=51820
		while [[ -n $(ip -4 route show table $table 2>/dev/null) || -n $(ip -6 route show table $table 2>/dev/null) ]]; do
			((table++))
		done
		wg set "$INTERFACE" fwmark $table
	fi
	local proto=-4 iptables=iptables pf=ip
	[[ $1 == *:* ]] && proto=-6 iptables=ip6tables pf=ip6
	ip $proto route add "$1" dev "$INTERFACE" table $table
	ip $proto rule add not fwmark $table table $table
	ip $proto rule add table main suppress_prefixlength 0

    # Things work without this, but it looks important

	# local marker="-m comment --comment \"wg-quick(8) rule for $INTERFACE\"" restore=$'*raw\n'
	# while read -r line; do
	# 	[[ $line =~ .*inet6?\ ([0-9a-f:.]+)/[0-9]+.* ]] || continue
	# 	printf -v restore '%s-I PREROUTING ! -i %s -d %s -m addrtype ! --src-type LOCAL -j DROP %s\n' "$restore" "$INTERFACE" "${BASH_REMATCH[1]}" "$marker"
	# done < <(ip -o $proto addr show dev "$INTERFACE" 2>/dev/null)
	# printf -v restore '%sCOMMIT\n*mangle\n-I POSTROUTING -m mark --mark %d -p udp -j CONNMARK --save-mark %s\n-I PREROUTING -p udp -j CONNMARK --restore-mark %s\nCOMMIT\n' "$restore" $table "$marker" "$marker"
	# [[ $proto == -4 ]] && sysctl -q net.ipv4.conf.all.src_valid_mark=1
	# echo -n "$restore" | $iptables-restore -n
	HAVE_SET_FIREWALL=1
	return 0
}

cmd_up() {
	local i
	trap 'del_if; exit' INT TERM EXIT
	# Get the allowed ips from wg show (added to peer using set_conf)
	for i in $(while read -r _ i; do for i in $i; do [[ $i =~ ^[0-9a-z:.]+/[0-9]+$ ]] && echo "$i"; done; done < <(wg show "$INTERFACE" allowed-ips) | sort -nr -k 2 -t /); do
		add_route "$i"
	done
	trap - INT TERM EXIT
}

cmd_down() {
	del_if
	remove_firewall || true
}


if [[ $# -eq 2 && $1 == up ]]; then
	cmd_up
elif [[ $# -eq 2 && $1 == down ]]; then
	cmd_down
else
	exit 1
fi

exit 0