var util = require('util');
var aTralics = require('../build/Release/tralics_main');

try
{
    aTralics.analyze("hello.tex", function(doc){
        console.log("parsed doc " + util.inspect(doc));
    });
}catch(e)
{
    console.log("EXCEPTION: " + util.inspect(e));
}