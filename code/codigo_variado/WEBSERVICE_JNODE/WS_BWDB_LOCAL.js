var express    = require('express');        // call express
var app        = express();                 // define our app using express
var bodyParser = require('body-parser');
var mysql      = require('mysql');

const util = require('util')

var f='ftp/BW_WIFI.log',
    fs=require('fs');

var strdate={
    jan:"01",
    feb:"02",    
    mar:"03",    
    apr:"04",    
    may:"05",    
    jun:"06",    
    jul:"07",    
    aug:"08",    
    sep:"09",    
    oct:"10",    
    nov:"11",    
    dec:"12"
}

app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

var port = process.env.PORT || 3000;        // set our port
var router = express.Router();              // get an instance of the express Router

router.get('/', function(req, res) {
    var data = req.query.data;
    //console.log("req:"+data);

    console.log(util.inspect(req.query.data, false, null))

    res.json({ message: "DATA["+data+"]" });
    fs.appendFile(f,data+'\n',function(err){
        if(err)
        console.error(err);
        console.log('['+data+'] Appended!');
    });
});


router.post('/', function(req, res) {
    var data = req.body // JSON.parse(req.body)
    var ipclient = req.headers['cf-connecting-ip'] || req.headers['x-forwarded-for'] || req.connection.remoteAddress

    if (typeof req.body === "undefined") {
    	console.log("Data is undefined")
    }
    else
	{

	    console.log("IP_CLIENT: "+ ipclient)

	    var reg_ip = /(\d+\.\d+\.\d+\.\d+)/
	    var reg_number = /([\d\.,]+)/
	    var reg_mdy = /(\w{3})\/(\d+)\/(\d+)/
	    var reg_datepart = /(\d+:\d+:\d+)/
	    var ymd=data.Datetime.match(reg_mdy)
	    var datepart=data.Datetime.match(reg_datepart)
	    var ymd=data.Datetime.match(reg_mdy)
	    var datepart=data.Datetime.match(reg_datepart)
	    var ip_client = ipclient.match(reg_ip)
	    
	    if(ip_client) ip_client=ip_client[0]
	      else
	       ip_client = '0.0.0.0'

	    var normdata={
		ip:data.Ip.match(reg_ip)[0],
		direction:data.Direction,
		duration:data.Duration,
		lostpackets:data['Lost-packets'].match(reg_number)[1],
		txcurrent:data['Tx-current'].match(reg_number)[1],
		tx10avg:data['Tx-10-second-average'].match(reg_number)[1],
		txtotalavg:data['Tx-total-average'].match(reg_number)[1],
		rxcurrent:data['Rx-current'].match(reg_number)[1],
		rx10avg:data['Rx-10-second-average'].match(reg_number)[1],
		rxtotalavg:data['Rx-total-average'].match(reg_number)[1],
		date_time:ymd[3]+'-'+strdate[ymd[1]]+'-'+ymd[2]+' '+datepart[0],
		ip_client:ip_client
	    }

	    //res.json({ message: "DATA["+JSON.stringify(normdata)+"]" });
	    fs.appendFile(f,JSON.stringify(data)+'\n',function(err){
		if(err) console.error(err);
		console.log(ip_client+'['+JSON.stringify(data)+'] Appended!');
	    });


	    var connection = mysql.createConnection({
		host     : 'localhost',
		user     : 'dbuser',
		password : 'xxxxus3r',
		database : "DB_MONITOR"
	    });

	    var sql = "INSERT INTO BW_DATA (ip, direction, duration, lostpackets, txcurrent, tx10avg, txtotalavg, rxcurrent, rx10avg, rxtotalavg, date_time, ip_client) VALUES ('"+
	    normdata.ip+"', '"+normdata.direction+"', '"+normdata.duration+"', '"+normdata.lostpackets+"', '"+
	    normdata.txcurrent+"', '"+normdata.tx10avg+"', '"+normdata.txtotalavg+"', '"+normdata.rxcurrent+"', '"+
	    normdata.rx10avg+"', '"+normdata.rxtotalavg+"', '"+normdata.date_time+"', '"+ip_client+"')"


		connection.query(sql, function (err, result) {

		        if(err)
		        {
		            res.send('Data add ' + " [" + JSON.stringify(data) + "]"); 
		        } 
		        else
		        {
		            res.send('Data added to database with ID: ' + result.insertId + " [" + JSON.stringify(data) + "]"); 
		        } 
		                    
		    }).on('error', function(err) {
		        console.log("[mysql error]",err);
		    });

		connection.end(); 

	    //console.log(sql)
	}

});

app.use('/test', router);

// START THE SERVER
// =============================================================================
app.listen(port);
console.log('Server start on port ' + port);

