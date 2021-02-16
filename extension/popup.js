class VPN {
  constructor() {
    this._p = document.getElementById('status');
    this.connect();

    document.getElementById('toggle').onclick = () => this.toggle();
    document.getElementById('showUI').onclick = () => this.send('show_ui');
  }

  t(msg) {
    this._p.innerText = msg;
  }

  connect() {
    let ws = new WebSocket('ws://localhost:8765');
    ws.onopen = () => {
      this.t('Connected!');
      this._ws = ws;
    } ws.onerror = () => {
      this.t('Connection failed');
      this._ws = null;
    } ws.onclose = () => {
      this.t('Disconnected');
      this._ws = null;
    } ws.onmessage = data => {
      if (data.data[0] == '!') {
        console.log(data.data);
        return;
      }

      if (data.data[0] == '@') {
        var msg = data.data.substring(1);
        const parts = msg.split('-');
        this._controllerState = parseInt(parts[1]);

        switch (parseInt(parts[0])) {
          case 0:
            this.t('Initialize');
            break;
          case 1:
            this.t('Authenticating');
            break;
          case 2:
            this.t('Post authenticating');
            break;
          case 3:
            switch (this._controllerState) {
              case 0:
                this.t('VPN initializing');
                break;
              case 1:
                this.t('VPN off');
                break;
              case 2:
                this.t('VPN connecting');
                break;
              case 3:
                this.t('VPN confirming');
                break;
              case 4:
                this.t('VPN on');
                break;
              case 5:
                this.t('VPN disconnecting');
                break;
              case 5:
                this.t('VPN switching');
                break;
            }
            break;
          case 4:
            this.t('Update required');
            break;
          case 5:
            this.t('Subscription needed');
            break;
          case 6:
            this.t('Validation subscription');
            break;
          case 7:
            this.t('Subscription blocked');
            break;
          case 8:
            this.t('Device limit');
            break;
          default:
            this.t('Unsupported state');
            break;
        }
      }
    }
  }

  send(msg) {
    if (!this._ws) return;
    this._ws.send(msg);
  }

  toggle() {
    if (this._controllerState == 1) {
      this.send('activate');
    }
    if (this._controllerState == 4) {
      this.send('deactivate');
    }
  }
};

const vpn = new VPN();
