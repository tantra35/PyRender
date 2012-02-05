from flup.server.fcgi import *;
#import fastcgi;
import Render;
import MySQLdb as dbi;
from MySQLdb.cursors import *;
import psyco;

def myapp(environ, start_response):
	dbh    = dbi.connect(named_pipe=True, db="playrix_corp", user="root", passwd="root");
	cursor = dbh.cursor();

	cursor.execute('SET CHARACTER SET utf8');
	cursor.close();

	write = start_response('200 OK', [('Content-Type', 'text/html; charset=utf-8')]);
	dict  = {};

	for k, v in environ.iteritems():
		dict[k] = v;

	render = Render.Render(dbh, dict);
	#render.render("D:/Data/src/PyRender/Debug/test.php", write);
	render.render("F:/Src/PyRender/Debug/test.php", write);

	return [];

psyco.full();
WSGIServer(myapp, bindAddress = ('192.168.20.4', 3345), debug=False).run();
#s = fastcgi.ThreadedWSGIServer(myapp, workers=5);
#s.serve_forever();