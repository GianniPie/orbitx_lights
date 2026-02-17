var ORBITX = { //collezione di macro (e di comandi per ciascuna macro) 
	"selectionofavatar":[
		//{ target:"tablet", cmd:"debug", txt:"Prova", expire:7000},
		{ target:"tablet", cmd:"selectedavatar", delay:0 },
		{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/default.webm", loop:true, delay:0 }, //riproduce video. Nel monitorF il video a cui ci riferiamo e' quello in background (nell'overlay rimane l'avatar scelto)
		{ target:"monitorf", cmd:"playoverlay", src:"assets/overlay{avatar}.webm", loop:true, delay:0 }, //pone l'avatar selezionato in overlay e lo visualizza
	],
	
	"reset":[ 
		{ target:"tablet|monitorf|monitora|monitorb|monitorc|monitord|desk1|desk2|desk3|desk4", cmd:"reset", delay:0},		
		{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/default.webm", loop:true, delay:0},
		{ target:"wsserver", cmd:"serial", txt:"RESET", delay:0 }, //reset (poi manda luce bianca)
	],
	
	"serial": [
		{ target:"wsserver", cmd:"serial", txt:"comando1", delay:4 }
	],
	
	"play":[
		{ target:"tablet", cmd:"play", delay:0},
		{ target:"wsserver", cmd:"serial", txt:"FIXALLA0FF7F;DIMALLUP04000;CMDSTART", delay:0 }, //luce blu
		{ target:"monitorf", cmd:"playoverlay", src:"assets/overlay{avatar}.webm", delay:0 }, //pone l'avatar selezionato in overlay e lo visualizza
		{ target:"monitorf", cmd:"playaudio", src:"assets/silent_spaceship.mp3", loop:true, delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		{ target:"monitorf", cmd:"playaudio", src:"assets/01_intro_g1_av{avatar}.mp3", delay:2 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		{ target:"wsserver", cmd:"serial", txt:"FIXTOPA0FF7F;FIXPFL60FF7F;FIXPFR60FF7F;FIXPRL60FF7F;FIXPRR60FF7F;WIT002;FIXTOPA0FF7F;CMDSTART", delay:24 }, //luce verde postazioni per 2 secondi
		{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/syserrloop.webm", loop:true, delay:36 }, //riproduce video di systemerror. Nel monitorF il video e' quello in background (nell'overlay rimane l'avatar scelto)
		{ target:"wsserver", cmd:"serial", txt:"FIXOCTA0FF7F;FIXTOPA0FF7F;ANMTOP200200FF7F;REP040;FIXTOP00FF7F;CMDSTART", delay:36 },//luce  rossa / blu (rimane rossa)
		
		//{ target:"wsserver", cmd:"serial", txt:"FIXTOP00FFFF", delay:5 },
		//{ target:"wsserver", cmd:"serial", txt:"CMDSTART", delay:5 },
		
		{ target:"desk1|desk2|desk3|desk4", cmd:"startgame", game:1, delay:115 },
		{ target:"wsserver", cmd:"serial", txt:"FIXTOP00FF7F;CMDSTART", delay:115 }, //luce rossa su laterali
		//{ target:"wsserver", cmd:"savephoto", delay:5 }, //scatta e salva foto - ...oppure si puo' lanciare una macro che scatta la foto { macroid:"savephoto", delay:4 }. E dovremo aggiungere quindi una macro "savephoto": [{ target:"wsserver", cmd:"savephoto", delay:4 }],
		//{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/default.webm",  loop:true, delay:9 }, //riproduce video. Nel monitorF il video a cui ci riferiamo e' quello in background (nell'overlay rimane l'avatar scelto)
		//{ target:"tablet", cmd:"allgamessolved", qrsvg:"{qrsvg}", delay:9 }, //la parte dei giochi e' terminata, sui desk verranno rimossi i giochi e visualizzato "qualcosa" in attesa del termine del viaggio. la chiave qrsvg viene valorizzata DA NODE, prima di inviare il comando, componendola sulla base di SCENARIO.folder. Es. se SCENARIO.folder e' 2025-10-01-11-22-33-100-afaf1, qrsvg sara' "2025/10/01/112233_100_afaf1.svg"
		//{ target:"tablet", cmd:"showqr", delay:10},
		//{ target:"wsserver", macroid:"reset", delay:15}, 
	],
	
	"game1suggestion1":[
		//{ target:"desk1|desk2|desk3|desk4", cmd:"requestedsuggestion", game:1, suggestion:1, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"showsuggestion", game:1, suggestion:1, delay:0 },
	  { target:"monitorf", cmd:"playaudio", src:"assets/02_g1_s1_av{avatar}.mp3", delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
	  //{ target:"tablet", cmd:"debug", txt:"richiesto suggerimento 1 per gioco1", expire:-1, delay:1},
	],
	"game1suggestion2":[
		//{ target:"desk1|desk2|desk3|desk4", cmd:"requestedsuggestion", game:1, suggestion:2, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"showsuggestion", game:1, suggestion:2, delay:0 },
		{ target:"monitorf", cmd:"playaudio", src:"assets/03_g1_s2_av{avatar}.mp3", delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		//{ target:"tablet", cmd:"debug", txt:"richiesto suggerimento 2 per gioco1", expire:-1, delay:1},
	],
	"game1suggestion3":[
		//{ target:"desk1|desk2|desk3|desk4", cmd:"requestedsuggestion", game:1, suggestion:3, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"showsuggestion", game:1, suggestion:3, delay:0 },
		{ target:"monitorf", cmd:"playaudio", src:"assets/04_g1_s3_av{avatar}.mp3", delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		//{ target:"tablet", cmd:"debug", txt:"richiesto suggerimento 3 per gioco1", expire:-1, delay:1},
	],	
	"game1solved":[
		{ target:"wsserver", cmd:"allmustsolve", solved:true, game:1, delay:0 }, //Il cmd "allmustsolve" innanzi tutto mostra che il gioco e' risolto sul desk che e' riuscito a risolverlo). Verra' passato anche un macro_owner corrispondente al desk ("desk1" o "desk2" etc...) che ha risolto il gioco. POI verifica che tutti e 4 i dsk abbiano risolto il gioco. Se cosi' e', verra' eseguita a sua volta la macro "gameXallsolved".
		//{ target:"tablet", cmd:"debug", txt:"un desk ha risolto il gioco 1", expire:-1, delay:1},
	],
	"game1ended":[
		{ target:"wsserver", cmd:"allmustsolve", solved:false, game:1, delay:0 }, //Il cmd "allmustsolve" innanzi tutto mostra che il gioco e' risolto sul desk che e' riuscito a risolverlo). Verra' passato anche un macro_owner corrispondente al desk ("desk1" o "desk2" etc...) che ha risolto il gioco. POI verifica che tutti e 4 i dsk abbiano risolto il gioco. Se cosi' e', verra' eseguita a sua volta la macro "gameXallsolved".
		//{ target:"tablet", cmd:"debug", txt:"gioco1 tempo scaduto", expire:-1, delay:1},	
	],
	"game1allsolved":[
		//{ target:"tablet", cmd:"debug", txt:"eseguita macro game1allsolved!", expire:-1, delay:1},
		{ target:"wsserver", cmd:"serial", txt:"FIXALLA0FF7F;DIMALLUP04000;CMDSTART", delay:0 }, //luce blu
		{ target:"wsserver", cmd:"savephoto", delay:0 }, //scatta e salva foto - ...oppure si puo' lanciare una macro che scatta la foto { macroid:"savephoto", delay:4 }. E dovremo aggiungere quindi una macro "savephoto": [{ target:"wsserver", cmd:"savephoto", delay:4 }],
		
		
		{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/default.webm", loop:true, delay:0 }, //fine video avaria e ritorno al video di default.
		{ target:"monitorf", cmd:"playaudio", src:"assets/05_g1_solved{countsolved}_av{avatar}.mp3", game:1, delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		{ target:"monitorf", cmd:"playaudio", src:"assets/06_intro_g2_av{avatar}.mp3", delay:8 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		
		{ target:"desk1|desk2|desk3|desk4", cmd:"reset", delay:15}, //~reset su unity
		
		{ target:"desk1|desk2|desk3|desk4", cmd:"startgame", game:2, delay:50 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		                                                                                                  
		//varie video e audio...
		//{ target:"desk1|desk2|desk3|desk4", cmd:"startgame", game:2, delay:5 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		
		//{ target:"desk1|desk2|desk3|desk4", cmd:"debug", txt:"all solved for game 3!!!", delay:4 }, //la parte dei giochi e' terminata, sui desk verranno rimossi i giochi e visualizzato "qualcosa" in attesa del termine del viaggio.
		//{ target:"desk1|desk2|desk3|desk4|tablet", cmd:"allgamessolved", qrsvg:"{qrsvg}", delay:3 }, //la parte dei giochi e' terminata, sui desk verranno rimossi i giochi e visualizzato "qualcosa" in attesa del termine del viaggio. la chiave qrsvg viene valorizzata DA NODE, prima di inviare il comando, componendola sulla base di SCENARIO.folder. Es. se SCENARIO.folder e' 2025-10-01-11-22-33-100-afaf1, qrsvg sara' "2025/10/01/112233_100_afaf1.svg"
		//{ target:"tablet", cmd:"showqr", delay:4},
		
		//{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/default.webm",  loop:true, delay:3 }, //riproduce video. Nel monitorF il video a cui ci riferiamo e' quello in background (nell'overlay rimane l'avatar scelto) 
		
		//{ macroid:"play2", delay:6}, 
	],
	
	"game2suggestion1":[
		//{ target:"desk1|desk2|desk3|desk4", cmd:"requestedsuggestion", game:2, suggestion:1, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"showsuggestion", game:2, suggestion:1, delay:0 },
	  { target:"monitorf", cmd:"playaudio", src:"assets/07_g2_s1_av{avatar}.mp3", delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
	  //{ target:"tablet", cmd:"debug", txt:"richiesto suggerimento 1 per gioco2", expire:-1, delay:1},
	],
	"game2suggestion2":[
		//{ target:"desk1|desk2|desk3|desk4", cmd:"requestedsuggestion", game:2, suggestion:2, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"showsuggestion", game:2, suggestion:2, delay:0 },
		{ target:"monitorf", cmd:"playaudio", src:"assets/08_g2_s2_av{avatar}.mp3", delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		//{ target:"tablet", cmd:"debug", txt:"richiesto suggerimento 2 per gioco2", expire:-1, delay:1},
	],
	"game2suggestion3":[
		//{ target:"desk1|desk2|desk3|desk4", cmd:"requestedsuggestion", game:2, suggestion:3, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"showsuggestion", game:2, suggestion:3, delay:0 },
		{ target:"monitorf", cmd:"playaudio", src:"assets/09_g2_s3_av{avatar}.mp3", delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		//{ target:"tablet", cmd:"debug", txt:"richiesto suggerimento 3 per gioco2", expire:-1, delay:1},
	],
	"game2solved":[
		{ target:"wsserver", cmd:"allmustsolve", solved:true, game:2, delay:0 }, 
		//{ target:"tablet", cmd:"debug", txt:"un desk ha risolto il gioco 2", expire:-1, delay:1},
		//{ target:"desk1|desk2|desk3|desk4", cmd:"startgame", game:3, delay:5 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro	
	],
	"game2ended":[
		{ target:"wsserver", cmd:"allmustsolve", solved:false, game:2, delay:0 }, //Il cmd "allmustsolve" innanzi tutto mostra che il gioco e' risolto sul desk che e' riuscito a risolverlo). Verra' passato anche un macro_owner corrispondente al desk ("desk1" o "desk2" etc...) che ha risolto il gioco. POI verifica che tutti e 4 i dsk abbiano risolto il gioco. Se cosi' e', verra' eseguita a sua volta la macro "gameXallsolved".
		//{ target:"tablet", cmd:"debug", txt:"gioco2 tempo scaduto", expire:-1, delay:1},	
	],
	"game2allsolved":[
		{ target:"wsserver", cmd:"savephoto", delay:0 }, //scatta e salva foto - ...oppure si puo' lanciare una macro che scatta la foto { macroid:"savephoto", delay:4 }. E dovremo aggiungere quindi una macro "savephoto": [{ target:"wsserver", cmd:"savephoto", delay:4 }],
		//{ target:"tablet", cmd:"debug", txt:"eseguita macro game2allsolved!", expire:-1, delay:1},
		{ target:"monitorf", cmd:"playaudio", src:"assets/10_g2_solved{countsolved}_av{avatar}.mp3", game:2, delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		
		{ target:"desk1|desk2|desk3|desk4", cmd:"reset", delay:6}, //~reset su unity
		{ target:"wsserver", cmd:"serial", txt:"FIXOCTA0FF7F;CMDSTART", delay:6 }, //luce blu ottagono frontale
		
		{ target:"monitorf", cmd:"playaudio", src:"assets/11_start.mp3", delay:6 }, //riproduce video di systemerror. Nel monitorF il video e' quello in background (nell'overlay rimane l'avatar scelto)
		{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/viaggio_part1.webm", delay:11 }, //riproduce video di systemerror. Nel monitorF il video e' quello in background (nell'overlay rimane l'avatar scelto)
		{ target:"monitorf", cmd:"playaudio", src:"assets/12_intro_g3_av{avatar}.mp3", delay:77 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		{ target:"wsserver", cmd:"serial", txt:"FIXTOPA0FF7F;ANMTOP2110A0FF7F;WIT003;FIXALL000000;FIXOCT00FF7F;WIT001;SHDLFT00FF7FA0FF7F;SHDRGT00FF7FA0FF7F;DIMTOPUP02000;CMDSTART", delay:77 }, //stelline blu per X tempo OCT rosso + sfumatura rosso davanti blu dietro
		
		{ target:"desk1|desk2|desk3|desk4", cmd:"startgame", game:3, delay:173 },
	],        	
	
	"game3suggestion1":[
		//{ target:"desk1|desk2|desk3|desk4", cmd:"requestedsuggestion", game:3, suggestion:1, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"showsuggestion", game:3, suggestion:1, delay:0 },
	  { target:"monitorf", cmd:"playaudio", src:"assets/13_g3_s1_av{avatar}.mp3", delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
	],
	"game3suggestion2":[
		//{ target:"desk1|desk2|desk3|desk4", cmd:"requestedsuggestion", game:3, suggestion:2, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"showsuggestion", game:3, suggestion:2, delay:0 },
		{ target:"monitorf", cmd:"playaudio", src:"assets/14_g3_s2_av{avatar}.mp3", delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
	],
	"game3solved":[
		{ target:"wsserver", cmd:"allmustsolve", solved:true, game:3, delay:0 }, //Il cmd "allmustsolve" innanzi tutto mostra che il gioco e' risolto sul desk che e' riuscito a risolverlo). Verra' passato anche un macro_owner corrispondente al desk ("desk1" o "desk2" etc...) che ha risolto il gioco. POI verifica che tutti e 4 i dsk abbiano risolto il gioco. Se così è, verrà eseguita a sua volta la macro "game3allsolved".
		//{ target:"tablet", cmd:"debug", txt:"un desk ha risolto il gioco 3", expire:-1, delay:1},  
	],
	"game3ended":[
		{ target:"wsserver", cmd:"allmustsolve", solved:false, game:3, delay:0 }, //Il cmd "allmustsolve" innanzi tutto mostra che il gioco e' risolto sul desk che e' riuscito a risolverlo). Verra' passato anche un macro_owner corrispondente al desk ("desk1" o "desk2" etc...) che ha risolto il gioco. POI verifica che tutti e 4 i dsk abbiano risolto il gioco. Se cosi' e', verra' eseguita a sua volta la macro "gameXallsolved".
		//{ target:"tablet", cmd:"debug", txt:"gioco3 tempo scaduto", expire:-1, delay:1},	
	],
	"game3allsolved":[
		{ target:"wsserver", cmd:"savephoto", delay:0 }, //scatta e salva foto - ...oppure si puo' lanciare una macro che scatta la foto { macroid:"savephoto", delay:4 }. E dovremo aggiungere quindi una macro "savephoto": [{ target:"wsserver", cmd:"savephoto", delay:4 }],
		//{ target:"tablet", cmd:"debug", txt:"eseguita macro game3allsolved!", expire:-1, delay:1},
		{ target:"monitorf", cmd:"playaudio", src:"assets/15_g3_solved{countsolved}_av{avatar}.mp3", game:3, delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro

		{ target:"desk1|desk2|desk3|desk4|tablet", cmd:"allgamessolved", qrsvg:"{qrsvg}", delay:3 }, //la parte dei giochi e' terminata, sui desk verranno rimossi i giochi e visualizzato "qualcosa" in attesa del termine del viaggio. la chiave qrsvg viene valorizzata DA NODE, prima di inviare il comando, componendola sulla base di SCENARIO.folder. Es. se SCENARIO.folder e' 2025-10-01-11-22-33-100-afaf1, qrsvg sara' "2025/10/01/112233_100_afaf1.svg"
		{ target:"tablet", cmd:"showqr", delay:4},
		{ target:"desk1|desk2|desk3|desk4", cmd:"showqr", delay:4},
		
		{ target:"desk1|desk2|desk3|desk4", cmd:"reset", delay:18}, //~reset su unity
		{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/viaggio_part3.webm", delay:13 }, //riproduce video di systemerror. Nel monitorF il video e' quello in background (nell'overlay rimane l'avatar scelto)
		{ target:"monitorf", cmd:"playaudio", src:"assets/16_return.mp3", delay:13 }, //riproduce video di systemerror. Nel monitorF il video e' quello in background (nell'overlay rimane l'avatar scelto)
		{ target:"wsserver", cmd:"serial", txt:"FIXTOPA0FF7F;ANMTOP2110A0FF7F;WIT003;FIXALL000000;WIT001;SHDLFT5555FFAAFFFF;SHDRGT5555FFAAFFFF;FIXOCT000000;DIMTOPUP05000;CMDSTART", delay:13 }, //stelline + qualcosa su ottagono peer xXtempo  luce Iniziale
		
		{ target:"wsserver", macroid:"reset", delay:90 },
		//{ target:"wsserver", cmd:"savephoto", delay:1 }, //scatta e salva foto - ...oppure si puo' lanciare una macro che scatta la foto { macroid:"savephoto", delay:4 }. E dovremo aggiungere quindi una macro "savephoto": [{ target:"wsserver", cmd:"savephoto", delay:4 }],
		//{ target:"desk1|desk2|desk3|desk4", cmd:"debug", txt:"all solved for game 3!!!", delay:4 }, //la parte dei giochi e' terminata, sui desk verranno rimossi i giochi e visualizzato "qualcosa" in attesa del termine del viaggio.
		
		//{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/default.webm",  loop:true, delay:3 }, //riproduce video. Nel monitorF il video a cui ci riferiamo e' quello in background (nell'overlay rimane l'avatar scelto) 
		//{ macroid:"play2", delay:15}, 
	],  	
	
	"jumptogame1": [
		//{ target:"monitorf", cmd:"stopallaudio", delay:0 }, //interrompe la riproduzione di TUTTI i suoni.
		{ target:"monitorf", cmd:"playaudio", src:"assets/silent_spaceship.mp3", loop:true, delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/syserrloop.webm", loop:true, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"reset", delay:0}, //~reset su unity
		{ target:"desk1|desk2|desk3|desk4", cmd:"startgame", game:1, delay:1 }, 	
	],
	
	"jumptogame2": [
		//{ target:"monitorf", cmd:"stopallaudio", delay:0 }, //interrompe la riproduzione di TUTTI i suoni.
		{ target:"monitorf", cmd:"playaudio", src:"assets/silent_spaceship.mp3", loop:true, delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/syserrloop.webm", loop:true, delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"reset", delay:0}, //~reset su unity
		{ target:"desk1|desk2|desk3|desk4", cmd:"startgame", game:2, delay:1 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
	],
	
	"jumptogame3": [
		//{ target:"monitorf", cmd:"stopallaudio", delay:0 }, //interrompe la riproduzione di TUTTI i suoni.
		{ target:"monitorf", cmd:"playaudio", src:"assets/silent_spaceship.mp3", loop:true, delay:0 }, //riproduce un audio. {avatar} verra' sostituito con l'avatar che viene inviato come parametro
		{ target:"monitorf|monitora|monitorb|monitorc|monitord", cmd:"playvideo", src:"assets/viaggio_part1.webm", delay:0 },
		{ target:"desk1|desk2|desk3|desk4", cmd:"reset", delay:0}, //~reset su unity
		{ target:"desk1|desk2|desk3|desk4", cmd:"startgame", game:3, delay:1 },
	],
	
	
	//alcuni comandi di esempio...
	"test":[ 
		{ target:"tablet", cmd:"debug", txt:"lorem ipsum 1", delay:1},
		{ target:"tablet", cmd:"debug", txt:"lorem ipsum 2", delay:1.5}, // ,expire:10000 //sul tablet questo messaggio scomparira' dopo 10 secondi invece che dopo i 7 secondi fissati di default in www/js/app.js)
		{ target:"wsserver", cmd:"savephoto", delay:2 }, //scatta e salva foto - ...oppure si puo' lanciare una macro che scatta la foto { macroid:"savephoto", delay:4 }. E dovremo aggiungere quindi una macro "savephoto": [{ target:"wsserver", cmd:"savephoto", delay:4 }],
		{ target:"wsserver", cmd:"serial", txt:"comando1", delay:3 }, //invio comando alla seriale - ...oppure si puo' lanciare una macro che invia comandi alla seriale { macroid:"serial", delay:4 }. E dovremo aggiungere quindi una macro "serial": [{ target:"wsserver", cmd:"serial", txt:"comando1", delay:1 }],
		//{ target:"wsserver", cmd:"reset", delay:4}, //NOTA: tutti i comandi con delay successivo a quello del "reset" verranno IGNORATI. il COMANDO "reset" dice a node (a wsserver) di eseguire la MACRO "reset". Non viene diffuso a tutti i clients: sara' node che, ricevuto questo comando, leggera' la MACRO "reset" che conterra' i comandi da inviare ai clients.
		{ target:"monitorf", cmd:"stopallaudio", delay:5 }, //interrompe la riproduzione di TUTTI i suoni.
	]
	//...fine comandi di esempio

}

if( typeof exports !== "undefined" && typeof module !== "undefined" && module.exports) {
	module.exports = ORBITX;
}    
