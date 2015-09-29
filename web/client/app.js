function getConfigData() {
  var options = {
    'show_remaining_mins': document.getElementById('show-remaining-minutes').checked,
    'show_padded_zero': document.getElementById('show-padded-zero').checked
  };

  // Save for next launch
  localStorage['show_remaining_mins'] = options['show_remaining_mins'];
  localStorage['show_padded_zero'] = options['show_padded_zero'];

  console.log('Got options: ' + JSON.stringify(options));
  return options;
}

function getQueryParam(variable, defaultValue) {
  var query = location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');
    if (pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || false;
}

$(document).ready(function() {
  document.getElementById('submit-button').addEventListener('click', function() {
    // Set the return URL depending on the runtime environment
    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(getConfigData()));
  });

  // Load any previously saved configuration, if available
  if (localStorage['show_remaining_mins']) {
    document.getElementById('show-remaining-minutes').checked = JSON.parse(localStorage['show_remaining_mins']);
    document.getElementById('show-padded-zero').checked = JSON.parse(localStorage['show_padded_zero']);
  } else {
    document.getElementById('show-remaining-minutes').checked = true;
    document.getElementById('show-padded-zero').checked = true;
  }
});
