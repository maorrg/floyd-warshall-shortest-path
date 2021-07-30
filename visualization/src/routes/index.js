const router = require('express').Router();
const path = require('path');
var bodyParser = require('body-parser');
router.use(bodyParser.json());

router.get('/', (req, res) => {
  res.render('index');
});


router.post('/', (req, res) => {
  res.render('index');
  console.log(req.body);
  writer(req.body.origen, req.body.destino);
});


function writer(origen, destino){
  fs = require('fs');
  var text = origen + "," +destino;
  fs.writeFile('public/files/out.txt', text, function (err) {
  if (err) return console.log(err);
  console.log('Archivo generado');
});
}

module.exports = router;
