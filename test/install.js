var tralic = require('tralics')
var util = require('util');

try
{
    tralic.analyze("hello.tex", function(doc){
        console.log("parsed doc " + util.inspect(doc));
    });
}catch(e)
{
    console.log("EXCEPTION: " + util.inspect(e));
}