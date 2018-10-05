/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package graphics;

import dialogs.MoreGamesWinnerDialog;
import dialogs.NewGameDialog;
import dialogs.WinnerDialog;
import frames.FourGames;
//import frames.OneGame;
import frames.OneGame;
import model.board.Game;
import model.cards.Card;
import model.cards.CardDeck;
import model.cards.CardDeckTarget;
import model.cards.CardStack;
import model.board.UndoStackItem;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import static java.awt.image.ImageObserver.WIDTH;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Objects;

/**
 * @author Roman
 */


public class CardPanel extends JPanel implements Serializable {
    private static final int WASTE = 0;
    private static final int TARGET = 1;
    private static final int WORKING = 2;
    private static final int ROTATE = 3;

    private BufferedImage img;
    private Card card;
    private Game game;

    private Container table;
    private Container stack_from;
    private Container stack_to;

    public Card getCard() {
        return card;
    }

    private CardPanel getCardPanel() {
        return this;
    }

    private void setStack_from(Container stack_from) {
        this.stack_from = stack_from;
    }

    private Container getStack_from() {
        return stack_from;
    }

    private void setTable(Container table) {
        this.table = table;
    }

    private Container getTable() {
        return table;
    }

    private void setStack_to(Container stack_to) {
        this.stack_to = stack_to;
    }

    private Container getStack_to() {
        return stack_to;
    }


    public CardPanel(Card card, Game game) {
        this.card = card;
        this.game = game;
        setOpaque(false);

        if (card == null) {
            try {
                img = ImageIO.read(getClass().getResource("/images/emptyCard.png"));
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        } else {
            try {
                if (card.isTurnedFaceUp())
                    img = ImageIO.read(getClass().getResource(card.getImage()));
                else
                    img = ImageIO.read(getClass().getResource("/images/Back.png"));
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }

        addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent me) {
                me.translatePoint(me.getComponent().getLocation().x, me.getComponent().getLocation().y);
                CardPanel p = getCardPanel();
                ArrayList<CardPanel> cards = new ArrayList<>();
                cards.add(p);
                int j = 0;
                
                if (p.getCard() == null)
                    return;
                else if (!p.getCard().isTurnedFaceUp())
                    return;
                
                Container stackPanel = getStack_from();
                
                if(stackPanel instanceof StackPanelInterface)
                {
                    StackPanel stackPanel1 = (StackPanel) stackPanel;
                    int index = stackPanel1.getCards().indexOf(p);
                    int maxIndex = stackPanel1.getCards().size() - 1;
                    
                    while(index < maxIndex)
                    {
                        index++;
                        cards.add(stackPanel1.getCards().get(index));
                        getTable().add(stackPanel1.getCards().get(index),0);
                    }
                }
                for(int i = 0; i < cards.size(); i++)
                {
                    cards.get(i).setLocation(me.getX(), me.getY() + j);
                    j += 20;
                }
                //p.setLocation(me.getX(), me.getY());
            }

            @Override
            public void mouseMoved(MouseEvent e) {
            }
        });


        addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent me) {
                CardPanel p = getCardPanel();

                if (p.getCard() == null)
                    return;
                else if (!p.getCard().isTurnedFaceUp())
                    return;

                setTable(p.getParent().getParent());
                setStack_from(p.getParent());

                getStack_from().repaint();
                getStack_from().remove((p));
                getTable().add(p, 0);

                System.out.println(getStack_from().getComponentCount());
            }


            @Override
            public void mouseReleased(MouseEvent me) {
                if (getCard() == null)
                    return;
                else if (!getCard().isTurnedFaceUp())
                    return;

                Container table = getTable();
                Container stack_from = getStack_from();

                table.setComponentZOrder(getCardPanel(), table.getComponentCount()-1);
                setStack_to((Container) table.getComponentAt(getLocation().getLocation()));

                if (getStack_to() == null) {
                    System.err.println("JE TO NULL !!!!!!!!!!!!!");
                    return_card();
                    return;
                }

                int to;
                Component cmp_to = table.getComponentAt(getLocation());
                CardDeck card_st_to;
                if (cmp_to instanceof DeckPanelInterface) {
                    card_st_to = ((DeckPanelInterface) cmp_to).getDeck();
                    System.out.println("položil som to na DECK");
                    to = TARGET;
                } else if (cmp_to instanceof StackPanelInterface) {
                    card_st_to = ((StackPanelInterface) cmp_to).getStack();
                    System.out.println("položil som to na STACK");
                    to = WORKING;
                } else {
                    System.out.println("položil som to mimo");
                    return_card();
                    return;
                }

                int from;
                CardDeck card_st_from;
                if (stack_from instanceof WastedPanel) {
                    card_st_from = ((WastedPanel) stack_from).getDeck();
                    System.out.println("bral som to z wastedpanelu");
                    from = WASTE;
                } else if (stack_from instanceof StackPanelInterface) {
                    card_st_from = ((StackPanelInterface) stack_from).getStack();
                    System.out.println("bral som to zo stackpanelu");
                    from = WORKING;
                } else if (stack_from instanceof TargetPanel) {
                    card_st_from = ((TargetPanel) stack_from).getDeck();
                    System.out.println("bral som to z targetPanelu");
                    from = TARGET;
                } else {
                    System.err.println("bral som to odniekiaľ inam -> koniec - branie z target ešte nefunguje");
                    return_card();
                    return;
                }

                if (card_st_to.checkPut(getCard())) {
                    System.out.println("   možeš to tam položiť");

                    put_card(card_st_from, card_st_to, from, to);
                } else {
                    System.out.println("   NEmožeš to tam položiť\n");
                    return_card();
                }

            }
        });
    }


    private void put_card(CardDeck card_dc_from, CardDeck card_dc_to, int from, int to) {
        CardStack card_st_from, s;
        Card c, c2;
        int size;
        boolean b = false;

        if (card_dc_from instanceof CardStack) {                                                                                                    System.out.println(" ======= bral som to zo stacku");

            card_st_from = (CardStack) card_dc_from;
            s = card_st_from.pop(this.card);
            size = s.size();

            c2 = card_st_from.get();
            if (c2 != null) {
                b = c2.turnFaceUp();
            }

            if (s.size() == 1) {
                card_dc_to.put(s.pop());
            }
            else if (card_dc_to instanceof CardStack) {                                                                                                 System.out.println(" +++++++ pokladam  to na stack");
                ((CardStack) card_dc_to).put(s);
            }
            else {                                                                                                                                        System.out.println(" +++++++ pokladam  to na  deck");
                if (s.size() > 1)
                    return_card();
                else {
                    if (s.get() == null)
                        System.err.println("nuuuuuuuuuuuuuuuul");
                    else
                        card_dc_to.put(s.get());
                    }
                }

            // uloženie ťahu
            this.game.pushItem(new UndoStackItem(card_st_from, card_dc_to, getStack_from(), getStack_to(), size, b, game.getScore()));
            // bodovanie
            this.game.scoring(from, to);
            if (b) this.game.scoring(ROTATE);
        }
        else {                                                                                                                                          System.out.println(" ======= bral som to z decku");
            c = card_dc_from.pop();
            card_dc_to.put(c);

            c2 = card_dc_from.get();
            if (c2 != null)
                b = c2.turnFaceUp();

            // uloženie ťahu
            this.game.pushItem( new UndoStackItem(card_dc_from, card_dc_to, getStack_from(), getStack_to(), b, game.getScore()));
            this.game.scoring(from, to);
        }

        Container xx = getStack_to();
        if (xx == null) {
            System.err.println("xx je null");
            return;
        }
        xx.add(getCardPanel());
        xx.revalidate();
        xx.repaint();

        Container game_con = getTable().getParent();
        while(!(game_con instanceof JFrame))
            game_con = game_con.getParent();
        if(game_con instanceof OneGame)
        {
            OneGame tmp_game = (OneGame) game_con;
            tmp_game.revalidateScore();
            tmp_game.revalidateScore();
        }
        else
        {
            FourGames tmp_game = (FourGames) game_con;
            int gameNum = tmp_game.getGameNumber(this.game);
            tmp_game.revalidateScore(gameNum);
            tmp_game.revalidateScore(gameNum);
        }

        getTable().remove(getCardPanel());
        getTable().revalidate();
        getTable().repaint();



        getStack_from().add(this);
        if(getStack_from() instanceof StackPanel)
        {
            StackPanel stack = (StackPanel) getStack_from();
            int index = stack.getCards().indexOf(this);
            int maxIndex = stack.getCards().size() - 1;
            
            while(index < maxIndex)
            {
                index++;
                getTable().remove(stack.getCards().get(index));
            }
        }
        
        getStack_from().remove(this);
        getStack_from().revalidate();
        getStack_from().repaint();

        if(this.game.checkWin())
        {
            Container con = getTable();
            while(!(con instanceof JFrame))
                con = con.getParent();
            
            if(con instanceof OneGame)
            {
                OneGame frame = (OneGame) con;
                
                NewGameDialog.Decision decision;
            
                WinnerDialog dialog = new WinnerDialog(frame, true);
                dialog.setLocationRelativeTo(null);
                dialog.setVisible(true);
                decision = dialog.getDecision();
                
                switch(decision)
                {
                    case CLOSE:
                        break;

                    case NEW:
                        frame.setGaming(false);
                        frame.setDefault();
                        frame.newGame();
                        break;

                    case EXIT:
                        frame.dispatchEvent(new WindowEvent(frame, WindowEvent.WINDOW_CLOSING));
                }
            }
            
            if(con instanceof FourGames)
            {
                FourGames frame = (FourGames) con;
                MoreGamesWinnerDialog dialog = new MoreGamesWinnerDialog(frame, true);
                dialog.setLocationRelativeTo(null);
                dialog.setVisible(true);
                int gameNum = frame.getGameNumber(this.game);
                if(gameNum != 0)
                {
                    frame.hidePanels(gameNum);
                    frame.setDefault(gameNum);
                    frame.cancelGaming(gameNum);
                }
                else
                    System.err.println("Hovadina!");
            }
        }

    }

    private void return_card() {
        getStack_from().add(getCardPanel());
        
        if(getStack_from() instanceof StackPanel)
        {
            StackPanel stack = (StackPanel) getStack_from();
            int index = stack.getCards().indexOf(this);
            int maxIndex = stack.getCards().size() - 1;
            
            while(index < maxIndex)
            {
                index++;
                getTable().remove(stack.getCards().get(index));
            }
        }

        getStack_from().revalidate();
        getStack_from().repaint();
        
        getTable().remove(getCardPanel());
        getTable().revalidate();
        getTable().repaint();
    }

    @Override
    public Dimension getPreferredSize() {
        return new Dimension(100, 150);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g.create();
        g2d.drawImage(img, 0, 0, null);
        g2d.dispose();
    }
}