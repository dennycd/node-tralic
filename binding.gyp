{
    'target_defaults' : {
        'conditions' : [
            #mac os
            ['OS=="mac"', {
             
                 'xcode_settings' : {
                    'OTHER_CPLUSPLUSFLAGS' : ["-fexceptions", "-stdlib=libstdc++"],
                    'GCC_ENABLE_CPP_EXCEPTIONS' : "YES",
                 }
             
             }],
                        
            #linux
            ['OS=="linux"', {
             
                #compiler flags
                'cflags!' : ["-fno-exceptions"],
                'cflags_cc!' : ["-fno-exceptions"],
                'cflags_cc' : ["-fexceptions"]
             }]
        ]
    },
    
    'targets' : [
        {
                 'target_name' : "tralics_main",
                 'sources' : ["lib/tralics_main.cpp", "lib/FlatLatexDoc.cpp", "lib/tralics/readline.cc", "lib/tralics/tralics.cc", "lib/tralics/txarray.cc", "lib/tralics/txatt.cc","lib/tralics/txaux.cc", "lib/tralics/txbib.cc","lib/tralics/txbuffer.cc", "lib/tralics/txcommands.cc", "lib/tralics/txerr.cc", "lib/tralics/txfonts.cc","lib/tralics/txfp.cc", "lib/tralics/txio.cc", "lib/tralics/txmath.cc", "lib/tralics/txmath1.cc", "lib/tralics/txmathboot.cc", "lib/tralics/txparser.cc", "lib/tralics/txparser3.cc", "lib/tralics/txparser2.cc", "lib/tralics/txpost.cc", "lib/tralics/txconfig.cc", "lib/tralics/txra.cc", "lib/tralics/txclasses.cc", "lib/tralics/txaccent.cc", "lib/tralics/txtitlepage.cc", "lib/tralics/txtrees.cc", "lib/tralics/txscan.cc", "lib/tralics/txtranslate.cc", "lib/tralics/txstack.cc", "lib/tralics/txmlio.cc", "lib/tralics/txtoken.cc", "lib/tralics/txtoken2.cc", "lib/tralics/txtoken1.cc", "lib/tralics/txmain.cc"]
        }
    ]
}