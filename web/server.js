var express = require('express');

var app = express();
var port = process.env.PORT || 3000;

// Set Jade as the templating engine
app.set('views', './views');
app.set('view engine', 'jade');

// Load front-end dependencies
app.use('/bower', express.static(__dirname + '/bower_components'));
app.use('/public', express.static(__dirname + '/client'));

app.get('/', function (req, res, next) {
  res.render("index");
});

// Host app
app.listen(port, function () {
  console.log('>> hosting on port', port);
});
