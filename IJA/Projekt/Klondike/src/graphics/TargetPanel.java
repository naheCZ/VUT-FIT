package graphics;

import model.board.Game;
import model.cards.CardDeck;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.Serializable;
import model.cards.CardDeckTarget;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * 
 * Graficka reprezentace ciloveho balicku. 
 */
public class TargetPanel extends JPanel implements DeckPanelInterface, Serializable
{


    private BufferedImage img;
    private CardDeck deck;
    private Game game;


    /**
     * Vrati cilovi balicek, prirazeny k tomuto objektu.
     * @return Cilovi balicek
     */
    @Override
    public CardDeck getDeck() {
        return deck;
    }

    /**
     * Nastavi cilovi balicek a hru, ke ktere patri.
     * @param deck Balicek karet
     * @param game Hra, ke ktere balicek patri
     */
    @Override
    public void setDeck(CardDeck deck, Game game) {
        this.deck = deck;
        this.game = game;
        this.revalidate();
    }
    
    /**
     * Konstruktor, ktery nastavi cilovi balicek. Ze zacatku bude cilovi balicek
     * reprezentovat prazdny obdelnik.
     */
    public TargetPanel()
    {
        super();
        setOpaque(false);
        setLayout(null);
        try {
            img = ImageIO.read(getClass().getResource("/images/emptyCard.png"));
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        
        this.deck = new CardDeckTarget();
    }
    
    /**
     * Konstruktor, ktery nastavi cilovi balicek. Ze zacatku bude cilovi balicek
     * reprezentovat prazdny obdelnika priradi hru, kte ktere cilovi balicek patri.
     * @param game Hra, ktere tento balicek patri
     */
    public TargetPanel(Game game)
    {       
        super();
        setOpaque(false);
        setLayout(null);
        try {
            img = ImageIO.read(getClass().getResource("/images/emptyCard.png"));
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        this.game = game;
    }

    @Override
    public void revalidate(){
        super.revalidate();
        if(this.deck != null)
            this.drawCards();
    }

    /**
     * Metoda, ktera do ciloveho balicku vykresli karty. Karty jsou polozeny primo
     * na sebe. 
     */
    public void drawCards()
    {
        this.setLayout(new OverlayLayout(this));

        this.removeAll();
        this.validate();
        
        for(int i = 0; i < this.deck.size(); i++)
        {
            CardPanel card = new CardPanel(this.deck.get(i), this.game);
            this.add(card, 0);
        }
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
}
