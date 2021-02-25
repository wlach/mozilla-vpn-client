/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

const assert = require('assert');
const util = require('util');
const vpn = require('./helper.js');
const FirefoxHelper = require('./firefox.js');

const webdriver = require('selenium-webdriver'), By = webdriver.By,
      Keys = webdriver.Key, until = webdriver.until;

const exec = util.promisify(require('child_process').exec);

describe('User authentication', function() {
  let driver;

  this.timeout(200000);

  before(async () => {
    await vpn.connect();
    driver = await FirefoxHelper.createDriver();
  });

  beforeEach(() => {});

  afterEach(() => {});

  after(async () => {
    await driver.quit();
    vpn.disconnect();
  });


  it('authenticate', async () => await vpn.authenticate(driver));

  it('Post authentication view', async () => {
    await vpn.waitForElement('postAuthenticationButton');
    await vpn.clickOnElement('postAuthenticationButton');
    await vpn.wait();
  });

  it('Main view', async () => {
    await vpn.waitForElement('controllerTitle');
    await vpn.waitForElementProperty('controllerTitle', 'visible', 'true');
  });

  it('open and close the settings', async () => {
    await vpn.waitForElement('settingsIcon');
    await vpn.waitForElementProperty('settingsIcon', 'visible', 'true');
    await vpn.clickOnElement('settingsIcon');

    await vpn.wait();

    await vpn.waitForElement('settingsBackIcon');
    await vpn.waitForElementProperty('settingsBackIcon', 'visible', 'true');
    await vpn.clickOnElement('settingsBackIcon');

    await vpn.wait();

    await vpn.waitForElement('controllerTitle');
    await vpn.waitForElementProperty('controllerTitle', 'visible', 'true');

    await vpn.wait();
  });

  it('settings -> manage account', async () => {
    await vpn.waitForElement('settingsIcon');
    await vpn.waitForElementProperty('settingsIcon', 'visible', 'true');
    await vpn.clickOnElement('settingsIcon');

    await vpn.wait();

    await vpn.waitForElement('manageAccountButton');
    await vpn.waitForElementProperty('manageAccountButton', 'visible', 'true');
    await vpn.clickOnElement('manageAccountButton');
    await vpn.waitForCondition(async () => {
      const url = await vpn.getLastUrl();
      return url.endsWith('/r/vpn/account');
    });

    await vpn.wait();
  });

  // TODO: start on boot
  // TODO: network settings
  // TODO: notification settings
  // TODO: languages
  // TODO: about us
  // TODO: give feedback
  // TODO: get help

  it('Logout', async () => {
    await vpn.waitForElement('signOutLink');
    await vpn.waitForElementProperty('signOutLink', 'visible', 'true');
    await vpn.clickOnElement('signOutLink');

    await vpn.wait();
  });

  it('quit the app', async () => await vpn.quit());
});
