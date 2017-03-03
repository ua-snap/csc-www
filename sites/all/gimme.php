<?php
$gimme = isset($_GET['gimme']) ? $_GET['gimme'] : false;
switch($gimme) {
  case 'db':
    exec('mysqldump -u drupal-readonly csc_drupal > /tmp/csc.sql');
    $file_url = '/tmp/csc.sql';
    header('Content-Type: application/octet-stream');
    header("Content-Transfer-Encoding: Binary");
    header("Content-disposition: attachment; filename=\"" . basename($file_url) . "\"");
    readfile($file_url);
    break;
  case 'files':
    exec('tar --exclude=\'*.mp4\' --exclude=\'*.mp3\' --exclude=\'*.pdf\' --exclude=\'*.ppt*\' -C /var/www/html/sites/default -cjvf /tmp/files-csc.bz2 files');
    $file_url = '/tmp/files-csc.bz2';
    header('Content-Type: application/octet-stream');
    header("Content-Transfer-Encoding: Binary");
    header("Content-disposition: attachment; filename=\"" . basename($file_url) . "\"");
    readfile($file_url);
    break;
  default:
  ?>
<h1>Fetch updated file zipball or database</h1>
  <ul>
    <li><a href="/sites/all/gimme.php?gimme=db">Gimme the database</a></li>
    <li><a href="/sites/all/gimme.php?gimme=files">Gimme the files zipball</a></li>
  </ul>
<?php
}
?>
