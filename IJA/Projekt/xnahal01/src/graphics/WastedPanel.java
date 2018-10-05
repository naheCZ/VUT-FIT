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
 * Graficka reprezentace odkladaciho balicku. 
 */
public class WastedPanel extends JPanel implements DeckPanelInterface, Serializable
{
    private BufferedImage img;
    private CardDeck deck;
    private Game game;

    /**
     * Vrati balicek karet, prirazeny k tomuto objektu.
     * @return Balicek karet
     */
    @Override
    public CardDeck getDeck() {
        return deck;
    }



    /**
     * Nastavi herni balicek a hru, ke ktere patri. 
     * @param deck Balicek karet
     * @param game Hra, ke ktere tento balicek patri.
     */
    @Override
    public void setDeck(CardDeck deck, Game game)
    {
        this.game = game;
        this.deck = deck;

        if (this.deck.size() != 0) {
            this.drawCards();
            this.setVisible(true);
        }
        else {
            this.removeCards();
            this.setVisible(false);
        }
    }

    /**
     * Konstruktor, ktery vykresli tento graficky objekt. Ze zacatku bude cilovi balicek
     * reprezentovat prazdny obdelnik.
     */
    public WastedPanel()
    {
        super();
        setOpaque(false);
        try {
            img = ImageIO.read(getClass().getResource("/images/emptyCard.png"));
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    
    /**
     * Konstruktor, ktery vykresli tento graficky objekt a priradi hru, ke tkere patri.
     * Ze zacatku bude cilovi balicek reprezentovat prazdny obdelnik.
     * @param game Hra, ktere tento balicek patri
     */
    public WastedPanel(Game game)
    {
        super();
        this.game = game;
        setOpaque(false);
        this.setLayout(new OverlayLayout(this));
        //setLayout(null);


    }
    
    /**
     * Odstraneni zobrazeni hranic grafickeho objektu.
     */
    public void removeBorder()
    {
        this.setBorder(null);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        if (img != null) {
            Graphics2D g2d = (Graphics2D) g.create();
            g2d.drawImage(img, 0, 0, null);

            g2d.dispose();
        }
    }
    


    @Override
    public void revalidate()
    {
        super.revalidate();
        if(this.deck != null)
            this.drawCards();
    }

    /**
     * Metoda, ktera vykresli karty, ktere jsou v tomto balicku. Karty jsou
     * vykreslovany primo na sebe.
     */
    public void drawCards()
    {
        this.setLayout(new OverlayLayout(this));
        CardPanel card = new CardPanel(this.deck.get(), this.game);
        this.add(card, 0);
        this.setVisible(true);
        this.repaint();
    }


    /**
     * Metoda, ktera odstrani vsechny karty v tomto balicku.
     */
    public void removeCards()
    {
        if (this.getComponentCount() != 0)
            this.removeAll();
        this.setVisible(false);

        this.repaint();
    }

    /**
     * Metoda, ktera vrati vice karet najednou- 
     */
    public void popCards(){
        while (deck.size()>0)
            deck.pop();
    }
}
