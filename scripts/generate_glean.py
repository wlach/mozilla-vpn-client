#! /usr/bin/env python3
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os.path
import yaml
import humps
import subprocess

print("Parsing the metrics.yaml file...")
if not os.path.isfile('glean/metrics.yaml'):
  print('Unable to find glean/metrics.yaml')
  exit(1)

yaml_file = open("glean/metrics.yaml", 'r')
yaml_content = yaml.safe_load(yaml_file)

print("Generating the C++ header...")
output = open("glean/generated/gleansample.h", "w")
output.write("""/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// AUTOGENERATED! DO NOT EDIT!!

namespace GleanSample {

""")

for key in yaml_content['sample']:
   sampleName = humps.camelize(key)
   output.write(f"constexpr const char* {sampleName} = \"{sampleName}\";\n")

output.write("""
} // GleanSample
""");
output.close();

def run_glean_parser(yaml):
  try:
    subprocess.call(["glean_parser", "translate", yaml, "-f", "javascript",
                     "-o", "glean/generated", "--option", "platform=qt",
                     "--option", "namespace=RealGlean"])
    return True
  except:
    print("glean_parser failed. Is it installed? Try with:\n\tnpm -g install glean_parser");
    return False

print("Generating the ping JS module...")
if not run_glean_parser("glean/pings.yaml"):
  exit(1)

print("Generating the sample JS module...")
if not run_glean_parser("glean/metrics.yaml"):
  exit(1)
