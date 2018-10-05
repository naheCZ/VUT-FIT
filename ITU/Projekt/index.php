<? header("Content-Type: text/html; charset=UTF-8");?>

<?php
  session_start();
  require('Db.php');
  Db::connect('localhost', 'xnahal01', 'xnahal01', 'om3gemsa');
  
  if (isset($_SESSION['uzivatel_id']))
  {
          header('Location: vytani.php');
          exit();
  }
  
  if ($_POST)
  {
          $uzivatel = Db::queryOne('
                  SELECT id, ucitel
                  FROM uzivatele
                  WHERE id=? AND heslo=?
          ', $_POST['id'], $_POST['heslo']);
          if (!$uzivatel)
                  $zprava = 'Neplatné uživatelské jméno nebo heslo';
          else
          {
                  $_SESSION['uzivatel_id'] = $uzivatel['id'];
                  $_SESSION['uzivatel_jmeno'] = $_POST['jmeno'];
                  $_SESSION['uzivatel_ucitel'] = $uzivatel['ucitel'];
                  header('Location: vytani.php');
                  exit();
          }
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
          <a href="index.php"><li class="aktivni">Přihlášení</li></a>
          <a href="zobrazit.php"><li>Moje konzultace</li></a>
          <a href="prochazet.php"><li>Dostupné konzultace</li></a>
          <a href="vytvareni.php"><li>Vytvořit konzultaci</li></a>
        </ul>
      </nav>
    </header>
    <article>
      <div id="centrovac">
      <section>
        <h2>Přihlášení do systému</h2>
        <?php
          if(isset($zprava))
            echo('<p>' . $zprava . '</p>');
        ?>
        <form method="post">
          Login: <input type="text" name="id" /><br /><br />
          Heslo: <input type="password" name="heslo" /><br /><br />
          <input type="submit" value="Přihlásit" />
        </form>
      </section>
      <div class="cistic"></div>
    </article>
    <footer>
      <p>Vytvořil Roman Nahálka v rámci projektu ITU 2016/2017 pro FIT VUT Brno</p>
    </footer>
  </body>
</html>
  
