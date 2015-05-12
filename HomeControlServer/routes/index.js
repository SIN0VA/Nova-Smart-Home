var express = require('express');
var router = express.Router();
var zmq = require('zmq');
var port = 'tcp://127.0.0.101:5555'; 
var socket = zmq.socket('push');
var requester = zmq.socket('req');
var b ={'temp':15,'humid':43};

socket.bindSync(port);
console.log('Home Web Control is conencted to port 5555 (to Smart Home Daemon - lights and switches control requests');
requester.connect('tcp://127.0.0.104:5559');
console.log('Home Web Control is conencted to port 5559 (to Smart Home Daemon - temperature and humidity requests)');
router.get('/', function(req, res, next) {
	//////////////////////////////////////////////////	
	requester.send("request");
	requester.on('message', function(msg) {
		console.log('Temp. and Humid. Request sent .', msg.toString());
		console.log('Got reply', msg.toString());
		var a = JSON.parse(msg.toString());	
		b.temp=a.temp;
		b.humid=a.humid;
	});
	res.render('index', { title: 'Smart Home Control' ,temp :b.temp  ,humid :b.humid,selected:'' });
	/////////////////////////////////////////
	
	console.log('index rendered');

});

router.post('/', function (req, res) {
	var light1 = req.body.switchid;
	var state1 = req.body.state;
	console.log('Switch : '+light1+' set to '+state1+ ' .');
	res.sendStatus(200);
	var json = {'switchid':light1, 'state':state1};
	console.log('bound!');
	console.log('Sending data ' + JSON.stringify(json));
	socket.send(JSON.stringify(json));
});


module.exports = router;


