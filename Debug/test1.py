import Render;
import MySQLdb as dbi;
from MySQLdb.cursors import *;

dbh    = dbi.connect(named_pipe=True, db="playrix_corp", user="root", passwd="1234");
cursor = dbh.cursor();
cursor.execute('SET CHARACTER SET utf8');
cursor.execute('select title from vfs_files where parent=%s', ("0"));
print cursor.description;
