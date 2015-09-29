var HEX_RADIX = 16;

Pebble.addEventListener('ready', function(e) {
  console.log('ready...');
});

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://christopherciufo.com:3030');
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

  var dict = {};
  dict['0'] = configData['show_remaining_mins'];
  dict['1'] = configData['show_padded_zero'];

  // Send to watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('Send failed!');
  });
});
