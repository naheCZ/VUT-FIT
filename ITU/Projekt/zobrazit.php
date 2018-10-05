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
  
  if($_POST)
  {   
    Db::query
    ('
      DELETE FROM reference
      WHERE ref_id=?
    ', $_POST['id']);
      
    Db::query
    ('
      UPDATE konzultace
      SET prihlaseno=prihlaseno - 1
      WHERE kon_id=?
    ', $_POST['kon_id']);
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
          <a href="zobrazit.php"><li class="aktivni">Moje konzultace</li></a>
          <a href="prochazet.php"><li>Dostupné konzultace</li></a>
          <a href="vytvareni.php"><li>Vytvořit konzultaci</li></a>
        </ul>
      </nav>
      <a href="zobrazit.php?odhlasit"><div id="off"><h1>Odhlásit se</h1></div></a>
    </header>
    <article>
      <div id = "centrovac">
      <section>
        <h2>Zapsané konzultace</h2>
        <?php
          $kon = Db::queryAll
          ('
            SELECT *
            FROM konzultace
          ');
          
          $zap = Db::queryAll
          ('
            SELECT *
            FROM reference
          ');
          
          echo('<table>');
          echo('<tr>');
          echo('<tr><th>Předmět</th>');
          echo('<th>Učitel</th>');
          echo('<th>Datum</th>');
          echo('<th>Začátek</th>');
          echo('<th>Konec</th>');
          echo('<th>Kapacita</th>');
          echo('<th>Přihlášeno</th>');
          echo('<th>Akce</th>');
          echo('</tr>');
          $u_id = htmlspecialchars($_SESSION['uzivatel_id']);
          $u_ty = htmlspecialchars($_SESSION['uzivatel_ucitel']);
          $b = 0;
          foreach ($kon as $k)
          {
            $k_id = htmlspecialchars($k['kon_id']);
            echo('<form method="post">');
            
            if(!$u_ty)
            {
              foreach ($zap as $z)
              {
                if($z['uzivatel'] == $u_id && $z['konzultace'] == $k_id)
                {
                  $c = $z['ref_id'];
                  echo('<tr><td>' . htmlspecialchars($k['predmet']));
                  echo('</td><td>' . htmlspecialchars($k['ucitel']));
                  echo('</td><td>' . htmlspecialchars($k['datum']));
                  echo('</td><td>' . htmlspecialchars($k['zacatek']));
                  echo('</td><td>' . htmlspecialchars($k['konec']));
                  echo('</td><td>' . htmlspecialchars($k['kapacita']));
                  echo('</td><td>' . htmlspecialchars($k['prihlaseno']));
                  echo('</td><td> <input type="submit" value="Odhlásit" />');
                  echo('<input type="hidden" name="operace" value="out" />');
                  echo('<input type="hidden" name="id" value="'. $c .'" />');
                  echo('<input type="hidden" name="kon_id" value="'. $k_id .'" />');
                }
              }
            }
            
            else
            {
              if($k['vytvoril'] == $u_id)
              {
                  echo('<tr><td>' . htmlspecialchars($k['predmet']));
                  echo('</td><td>' . htmlspecialchars($k['ucitel']));
                  echo('</td><td>' . htmlspecialchars($k['datum']));
                  echo('</td><td>' . htmlspecialchars($k['zacatek']));
                  echo('</td><td>' . htmlspecialchars($k['konec']));
                  echo('</td><td>' . htmlspecialchars($k['kapacita']));
                  echo('</td><td>' . htmlspecialchars($k['prihlaseno']));
                  echo('</td><td> <input type="submit" value="Odstranit" />');
                  echo('<input type="hidden" name="operace" value="delete" />');
                  echo('<input type="hidden" name="kon_id" value="'. $k_id .'" />');
                  echo('</td><td><a href="vytvareni.php?upravit=' . $k_id .'"><button type="button">Upravit</button></a>');
                  echo('</td><td><a href="prihlaseni.php?id=' . $k_id .'"><button type="button">Přihlášeni</button></a>');  
              }
            }
            echo('</form>');
            echo('</td></tr>');
          }
          echo('</table>');
        ?>
        <br /><br />
      </section>
      <div class="cistic"></div>
      </div>
      </article>
      <footer>
      <p>Vytvořil Roman Nahálka v rámci projektu ITU 2016/2017 pro FIT VUT Brno</p>
    </footer>
    </body>
</html>