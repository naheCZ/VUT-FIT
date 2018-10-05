package graphics;

import model.board.Game;
import model.cards.CardStack;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * 
 * Graficka reprezentace herniho balicku. 
 */
public class StackPanel extends JPanel implements StackPanelInterface, Serializable
{
    private BufferedImage img;
    private CardStack stack;
    private Game game;
    private ArrayList<CardPanel> cards = new ArrayList<>();

    /**
     * Metoda, ktera vrati tento objekt.
     * @return Graficka reprezentace hraciho balicku
     */
    private StackPanel getStackPanel(){
        return this;
    }
    
    /**
     * Konstruktor, ktery vytvori zakladni reprezentaci herniho balicku bez karet.
     */
    public StackPanel()
    {
        super();
        setOpaque(false);
        setLayout(null);
        this.setBorder(BorderFactory.createLineBorder(Color.red));
    }
    
    /**
     * Nastavi grafickou reprezentaci hraciho balicku do vychozi polohy.
     * @param game Hra, ke ktere je hraci balicek prirazen.
     */
    public StackPanel(Game game)
    {       
        super();
        setOpaque(false);
        try {
            img = ImageIO.read(getClass().getResource("/images/emptyCard.png"));
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        setLayout(null);
        this.setBorder(BorderFactory.createLineBorder(Color.red));
        this.game = game;
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
     * Odstraneni zobrazeni hranic grafickeho objektu.
     */
    public void removeBorder()
    {
        this.setBorder(null);
    }

    /**
     * Nastavi prirazeny herni balicek a hru, kte ktere se vaze a pote tento 
     * balicek vykresli. 
     * @param stack Zasobnik karet, prirazeny k tomuto objektu.
     * @param game Hra, ke ktere tento hraci balicek patri.
     */
    @Override
    public void setStack(CardStack stack, Game game)
    {
        this.game = game;
        this.stack = stack;
        this.removeBorder();
        this.drawCards();
    }

    /**
     * Vrati zasobnik karet, ktery je prirazen k tomuto objektu.
     * @return Zasobnik karet
     */
    @Override
    public CardStack getStack() {
        return stack;
    }

    /**
     * Vrati pole grafickych objektu, ktere repezentuji karty v tomto hernim balicku.
     * @return Pole graficke reprezentace karet
     */
    public ArrayList<CardPanel> getCards()
    {
        return this.cards;
    }
    
    @Override
    public void revalidate(){
        super.revalidate();

        if(this.stack != null)
            this.drawCards();
    }

    /**
     * Metoda pro vykresleni herniho balicku. Metoda vykresli karty pekne pod 
     * sebou, aby bylo mozne vide a manipulovat s kteroukoliv otocenou kartou v 
     * balicku. 
     */
    public void drawCards()
    {               
        int j = 0;
        int i = 0;

        if (this.stack.size() == 0){
            this.removeAll();
            this.validate();
            this.repaint();
            return;
        }
        
        this.removeAll();
        this.cards.clear();
        this.validate();
        
        for(i = 0; i < this.stack.size(); i++)
        {
            CardPanel card = new CardPanel(this.stack.get(i), this.game);
            card.setBounds(0, j, 100, 150);
            this.add(card, 0);
            this.cards.add(card);
            j += 20;
        }
        
        this.repaint();
        this.setPreferredSize(new Dimension(100, 150 + i * 20));
    }
}
