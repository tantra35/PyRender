from flup.server.fcgi import *;
import Render;
import MySQLdb as dbi;
from MySQLdb.cursors import *;

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

WSGIServer(myapp, bindAddress = ('localhost', 3345), debug=False).run();