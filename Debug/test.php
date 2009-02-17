<html>
<head>
<?php
include 'test.z';

function printdata($depth, $data)
{
	//print str_repeat("\t", $depth);
	printf ("<b>%s</b>\n", get_class($data));

	foreach($data as $key => $val)
	{
		if(!is_object($val))
		{
			print str_repeat("\t", $depth + 1);
			printf("%s => %s\n", $key, $val);
		}
		else
		{
			print str_repeat("\t", $depth + 1);
			printf("%s => ", $key);
			printdata($depth + 1, $val);
		};
	};
};
?>	
<script>
function test()
{
	alert('werwer');
};
</script>
</head>
<body>
<input type='button' value='press' onclick='test()'>
<h1>Python rules</h1>
<?php
//print_r($_SERVER['PyContext']);
$dbconnection = $_SERVER['PyContext']->get_connection();
$cursor = $dbconnection->cursor();

print_r($cursor);

$cursor->execute('SET CHARACTER SET utf8');

foreach($cursor->description as $key => $val)
{
	printf("%s\n", get_class($val));
	
	foreach($val as $key => $item)
	{
		printf("\t%s => %s\n", $key, $item);
	};
};	

print $cursor->execute('select * from vfs_files where parent=%s', array(0));
?>
<pre>
<?php
/*
foreach($cursor->description as $key => $val)
{
	printf("%s\n", get_class($val));
	
	foreach($val as $key => $item)
	{
		printf("\t%s => %s\n", $key, $item);
	};
};
*/
?>
</pre>

<pre>
<?php
$rows = $cursor->fetchmany(10);
printdata(0, $rows);
//$rows = $cursor->fetchall();

//print_r($rows);

/*
for($i=0; $row = $cursor->fetchone(); $i++)
{
	print '<pre>';
	printdata(0, $row);
	print '</pre>';
};
*/
?>
</pre>
<h3>here is small text fo templating</h3>
<pre>
<?php
$data = $_SERVER['PyContext']->get_data();
printdata(0, $data);

?>
zoooooooooooooooooooooooooooooooooooooooooo
</pre>
<h4>А проверю ка я русский текст</h4>
</body>
</html>
	