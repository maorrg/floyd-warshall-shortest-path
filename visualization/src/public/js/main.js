var map;
var flag = false;
var origenId;
var destinoId;


function readFile(){
    var urls = ["./files/nodes.txt"];
    xhrDoc= new XMLHttpRequest();   
    xhrDoc.open('GET', urls[0] )
    if (xhrDoc.overrideMimeType)
        xhrDoc.overrideMimeType('text/plain; charset=x-user-defined')
    xhrDoc.onreadystatechange =function()
    {
    if (this.readyState == 4)
    {
    if (this.status == 200)
   {
    const data = this.response;
    parser(data);
    return data;
   }

}                   
}
xhrDoc.send();
}

function parser(string){
    nodes = [];
    var myText = string;
    var lines = myText.split("\n");
    var numLines = lines.length;
    var id = '';
    var lat = '';
    var lng = '';
    var i = 0;
    var currentLine = 0;
    var tempNode = Array();
    

    var line;
    while(currentLine < numLines){
        line = lines[currentLine];
        
        while(line[i] != ','){
            id += line[i];
            i++;
        }
        i++;
        while(line[i] != ','){
            lat += line[i];
            i++;
        }

        i++;
        while(i < line.length){
            lng += line[i];
            i++;
        }
        
        i = 0;
        currentLine++;
        tempNode.push(id); tempNode.push(lat); tempNode.push(lng);
        nodes.push(tempNode);
        tempNode = [];
        id = '';
        lat = '';
        lng = '';
    }
    addAllNodes(nodes);
    console.log(nodes);
}


function addNode(latitutd, longitud, id){

    var node = L.circle([latitutd,longitud, id], {
        color: 'red',
        fillColor: '#f03',
        fillOpacity: 0.5,
        radius: 3
    }).addTo(map).on("click", nodeClick);
}

function sendRequest(begin, end){
    const params = {
        origen:begin,
        destino: end
    };
    const http = new XMLHttpRequest()
    http.open('POST', 'http://127.0.0.1:3000/')
    http.setRequestHeader('Content-type', 'application/json')
    http.send(JSON.stringify(params))
    http.onload = function() {
        //alert("Oringen ID: " + params.origen + "\nDestino ID: " + params.destino);

    }
}


function addAllNodes(nodes) {
    var lat, lng;
    setView(nodes[6][1], nodes[6][2]);
    for(var i=0; i < nodes.length; i++ ){
        lat = nodes[i][1];
        lng = nodes[i][2];
        id = nodes[i][0];
        addNode(lat,lng,id);
    }
}


function nodeClick(e) {
    const id = e.latlng.alt;
    const lat = e.latlng.lat;
    const lng = e.latlng.lng;

    var clickedCircle = e.target;
    if(!flag){
        clickedCircle.bindPopup("Origen").openPopup();
        flag = true;
        origenId = id;
    }else{
        clickedCircle.bindPopup("Destino").openPopup();
        destinoId = id;
        flag = false;
        sendRequest(origenId,destinoId);

        setTimeout(function() {
            location.reload();
        }, 400);
    }
    console.log("Id: "+id +"\nLat: "+ lat + "\nLng: " + lng);
}


function readWays(){
    var urls = ["./files/path.txt"];
    xhrDoc= new XMLHttpRequest();   
    xhrDoc.open('GET', urls[0] )
    if (xhrDoc.overrideMimeType)
        xhrDoc.overrideMimeType('text/plain; charset=x-user-defined')
    xhrDoc.onreadystatechange =function()
    {
    if (this.readyState == 4)
    {
    if (this.status == 200)
   {
    const data = this.response;
    parseWays(data);
    }
}
}
xhrDoc.send();
}

function parseWays(string){
    nodes = [];
    var myText = string;
    var lines = myText.split("\n");
    var numLines = lines.length;
    var id = '';
    var lat = '';
    var lng = '';
    var i = 0;
    var currentLine = 0;
    var tempNode = Array();
    var line;
    while(currentLine < numLines){
        line = lines[currentLine];
        
        while(line[i] != ','){
            id += line[i];
            i++;
        }
        i++;
        while(line[i] != ','){
            lat += line[i];
            i++;
        }

        i++;
        while(i < line.length){
            lng += line[i];
            i++;
        }
        
        i = 0;
        currentLine++;
        tempNode.push(lat); tempNode.push(lng);
        nodes.push(tempNode);
        tempNode = [];
        id = '';
        lat = '';
        lng = '';
    }
    addAllWays(nodes);
    console.log(nodes);
}



function addAllWays(nodes){
    var lat, lng, id, prev;
    var edgess = [];
    for(var i = 0; i < nodes.length; i++){
        edgess.push(nodes[i]);
        edgess.push(nodes[++i]);
        addPath(edgess);
        edgess = [];
    }
}



function addPath(edges){
    var begin, end;
    begin = edges[0];
    end = edges[1];
    console.log(begin, end);
    addLine(begin,end);
}


function addLine(begin,end){
    var path = L.polygon([
        begin,
        end
    ]).addTo(map);
}

function setView(lat, lng){
    map = L.map('map-template').setView([lat, lng], 15);
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
}).addTo(map);
}


function main(){
    readFile();
    readWays();
}

main();
