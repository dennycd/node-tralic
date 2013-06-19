## node-tralic
A node.js module for converting latex source to javascript json. It is a wrapper based on the following c library 

* TRALICS <http://www-sop.inria.fr/marelle/tralics/>


To install the package in npm 
```javascript
npm install tralics
```

To analyze a latex file 
```javascript 
var tralic = require('tralics');
var util = require('util');

tralic.analyze("hello.tex", function(doc){
    console.log("DOC " + util.inspect(doc));
});
```
