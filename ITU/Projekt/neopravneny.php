<? header("Content-Type: text/html; charset=UTF-8");?>
<?php
  session_start();
  if(!isset($_SESSION['uzivatel_id']))
  {
    header('Location: index.php');
    exit();
  }
  
  if(isset($_GET['odhlasit']))
  {
    session_destroy();
    header('Location: index.php');
    exit();
  }
?>

<!DOCTYPE html>
<html lang="cs-cz">
  <head>
    <meta charset="utf-8" />
    <title>Konzultační systém</title>
    <link rel="stylesheet" type="text/css" href="styly.css">
    <script type="text/javascript" src="skripty.js"></script>
  </head>
  <body>
    <header>
      <div id="logo"><h1>Správa konzultačních schůzek</h1></div>
      <nav>
        <ul>
          <a href="index.php"><li>Přihlášení</li></a>
          <a href="zobrazit.php"><li>Moje konzultace</li></a>
          <a href="prochazet.php"><li>Dostupné konzultace</li></a>
          <a href="vytvareni.php"><li>Vytvořit konzultaci</li></a>
        </ul>
      </nav>
      <a href="neopravneny.php?odhlasit"><div id="off"><h1>Odhlásit se</h1></div></a>
    </header>
    <article>
      <div id = "centrovac">
      <section>
        <h2>Chyba!</h2>
        <p>Studenti nemohou vytvářet konzultace!</p>
      </section>
      <div class="cistic"></div>
      </div>
      </article>
      <footer>
      <p>Vytvořil Roman Nahálka v rámci projektu ITU 2016/2017 pro FIT VUT Brno</p>
    </footer>
    </body>
</html>