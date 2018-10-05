package graphics;

import model.board.Game;
import model.cards.CardDeck;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.Serializable;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * 
 * Graficka reprezentace hlavniho herniho balicku. 
 */
public class MainDeckPanel extends JPanel implements DeckPanelInterface, Serializable
{
    private BufferedImage img;
    private CardDeck deck;
    private Game game;


    /**
     * Vrati balicek karet, prirazeny k jeho graficke reprezentaci.
     * @return Balicek karet
     */
    @Override
    public CardDeck getDeck() {
        return deck;
    }

    /**
     * Nastavi herni balicek a hru, ke ktere patri.
     * @param deck Balicek karet
     * @param game Hra, kte ktere balicek patri
     */
    @Override
    public void setDeck(CardDeck deck, Game game)
    {
        this.deck = deck;
        this.game = game;
    }

    /**
     * Zakladni konstruktor balicku, ktery na herni pole vykresli zadni cast karty.
     */
    public MainDeckPanel()
    {
        setOpaque(false);

        try {
            img = ImageIO.read(this.getClass().getResource("/images/Back.png"));
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    
    @Override
    protected void paintComponent(Graphics g)
    {        
        super.paintComponent(g);

        if(img != null)
        {
            Graphics2D g2d = (Graphics2D) g.create();
            g2d.drawImage(img, 0, 0, null);
        
            g2d.dispose();
        }
    }
    



    /**
     * Nastaveni obrazku, ktery se bude vykreslovat na herni pole. Bud se jedna o
     * obrazek zadni casi karty nebo o tlacitko, ktere uzivatele vyzve k znovu
     * pretoceni balicku.
     */
    public void setImg(){
        this.setBorder(null);
        try
        {
            if(this.deck.size() == 0)
                img = ImageIO.read(getClass().getResource("/images/reload2.png"));
            else
                img = ImageIO.read(getClass().getResource("/images/Back.png"));
        }
        catch (IOException ex)
        {
            ex.printStackTrace();
        }
        this.validate();
    }

}
