<? header("Content-Type: text/html; charset=UTF-8");?>

<?php
session_start();
if (empty($_SESSION['uzivatel_ucitel']))
{
  header('Location: neopravneny.php');
  exit();
}

require('Db.php');
Db::connect('localhost', 'xnahal01', 'xnahal01', 'om3gemsa');

$konzult = array(
        'kon_id' => '',
        'predmet' => '',
        'ucitel' => '',
        'datum' => '',
        'zacatek' => '',
        'kapacita' => '',
        'prihlaseno' => '',
        'konec' => '',
        'vytvoril' => '',
);
if ($_POST)
{
        if (!$_POST['kon_id'])
        {
                Db::query('
                        INSERT INTO konzultace (predmet, ucitel, datum, zacatek, kapacita, konec, vytvoril)
                        VALUES (?, ?, ?, ?, ?, ?, ?)
                ', $_POST['predmet'], $_POST['ucitel'], $_POST['datum'], $_POST['zacatek'], $_POST['kapacita'], $_POST['konec'], $_SESSION['uzivatel_id']);
        }
        else
        {
                Db::query('
                        UPDATE konzultace
                        SET predmet=?, ucitel=?, datum=?, cas=?, kapacita=?, konec=?
                        WHERE kon_id=?
                ', $_POST['predmet'], $_POST['ucitel'], $_POST['datum'], $_POST['zacatek'], $_POST['kapacita'], $_POST['konec'], $_POST['kon_id']);
        }
        header('Location: vytvareni.php');
        exit();
}

else if(isset($_GET['odhlasit']))
  {
    session_destroy();
    header('Location: index.php');
    exit();
  }
  
else if(isset($_GET['upravit']))
{
  $nacKon = Db::queryOne
  ('
    SELECT *
    FROM konzultace
    WHERE kon_id=?
  ', $_GET['upravit']);
  
  $konzult = $nacKon;
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
          <a href="vytvareni.php"><li class="aktivni">Vytvořit konzultaci</li></a>
        </ul>
      </nav>
      <a href="vytvareni.php?odhlasit"><div id="off"><h1>Odhlásit se</h1></div></a>
    </header>
    <article>
      <div id="centrovac">
      <section>
        <h2>Vytvoření konzultace</h2>
        <?php
          if(isset($zprava))
            echo('<p>' . $zprava . '</p>');
        ?>
        <form method="post">
          <input type="hidden" name="kon_id" value="<?= htmlspecialchars($konzult['kon_id']) ?>" />
          Předmět: <br /> 
          <input type="text" name="predmet" value="<?= htmlspecialchars($konzult['predmet']) ?>" /><br /><br />
          Učitel: <br /> 
          <input type="text" name="ucitel" value="<?= htmlspecialchars($konzult['ucitel']) ?>" /><br /><br />
          Datum: <br /> 
          <input type="date" name="datum" value="<?= htmlspecialchars($konzult['datum']) ?>" /><br /><br />
          Začátek: <br /> 
          <input type="time" name="zacatek" value="<?= htmlspecialchars($konzult['zacatek']) ?>" /><br /><br />
          Konec: <br />
          <input type="time" name="konec" value="<?= htmlspecialchars($konzult['konec']) ?>" /><br /><br />
          Kapacita: <br /> 
          <input type="text" name="kapacita" value="<?= htmlspecialchars($konzult['kapacita']) ?>" />
          <br /><br />
          <input type="submit" value="Odeslat" />
        </form>
      </section>
      <div class="cistic"></div>
    </article>
    <footer>
      <p>Vytvořil Roman Nahálka v rámci projektu ITU 2016/2017 pro FIT VUT Brno</p>
    </footer>
  </body>
</html>