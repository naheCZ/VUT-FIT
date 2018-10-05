<? header("Content-Type: text/html; charset=UTF-8");?>
<?php
  require_once('Db.php');
  Db::connect('localhost', 'xnahal01', 'xnahal01', 'om3gemsa');
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
  
  if(isset($_GET['id']))
  {
    $nacKon = Db::queryOne
    ('
      SELECT *
      FROM konzultace
      WHERE kon_id=?
    ', $_GET['id']);
    
    $konzult = $nacKon;
    $k_id = $_GET['id'];
  }
  
  else
  {
    header('Location: zobrazit.php');
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
      <a href="vytani.php?odhlasit"><div id="off"><h1>Odhlásit se</h1></div></a>
    </header>
    <article>
      <div id = "centrovac">
      <section>
        <h2>Přihlášení uživatelé</h2>
        <?php
          $zap = Db::queryAll
          ('
            SELECT *
            FROM reference
          ');
          
          $uzi = Db::queryAll
          ('
            SELECT *
            FROM uzivatele
          ');          
          echo('<table>');
          echo('<tr>');
          echo('<tr><th>Jméno</th>');
          echo('<th>Přijmení</th>');
          echo('<th>Login</th>');
          echo('</tr>');
          foreach($zap as $z)
          {
            if($z['konzultace'] == $k_id)
            {
              $u = Db::queryOne
              ('
                SELECT *
                FROM uzivatele
                WHERE id=?
              ', $z['uzivatel']);
              
              echo('<tr><td>' . htmlspecialchars($u['jmeno']));
              echo('</td><td>' . htmlspecialchars($u['prijmeni']));
              echo('</td><td>' . htmlspecialchars($u['id']));  
            }
            
            echo('</form>');
            echo('</td></tr>');
          }
          echo('</table>');          
        ?>
      </section>
      <div class="cistic"></div>
      </div>
      </article>
      <footer>
      <p>Vytvořil Roman Nahálka v rámci projektu ITU 2016/2017 pro FIT VUT Brno</p>
    </footer>
    </body>
</html>