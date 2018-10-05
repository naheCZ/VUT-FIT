package model.board;

import model.cards.*;

import javax.swing.*;
import java.io.*;
import java.util.ArrayList;
import java.util.Stack;

/**
 * project: ija_projekt
 * author: Adam Zivcak
 * login:  xzivca03
 * created: 1.4.2017
 */
public class KlondikeGame implements Game, Serializable {
    private static final int WASTE = 0;
    private static final int TARGET = 1;
    private static final int WORKING = 2;
    private static final int ROTATE = 3;
    private static final int RECYCLE = 4;

    private static final long serialVersionUID = 1L;

    private ArrayList<CardStack> tableau = new ArrayList<>(7);
    private ArrayList<CardDeckTarget> foundations = new ArrayList<>(4);
    private CardDeck mainDeck;
    private CardDeck waste = new CardDeckKlondike();
    private int score = 0;
    private Stack<UndoStackItem> undoStack = new Stack<>();

    private FactoryKlondike factory = new FactoryKlondike();

    @Override
    public void pushItem(UndoStackItem i) {
        this.undoStack.push(i);
        System.out.println("\n +++ počet ťahov: "+undoStack.size() + "\n");
    }

    @Override
    public UndoStackItem popItem() {
        if(this.undoStack.size() > 0){
            return this.undoStack.pop();
        }
        else
            return null;
    }

    @Override
    public boolean checkWin() {
        /*for (int i = 0; i < 7; i++) {
            if (tableau.get(i).size() != 0)
                return false;
        }
        if (mainDeck.size() != 0)
            return false;
        if (waste.size() != 0)
            return false;*/

        for (int i = 0; i < 4; i++) {
            if(foundations.get(i).size() != 13)
                return false;
        }
        return true;
    }


    @Override
    public Game getGame() {
        return this;
    }

    @Override
    public Stack<UndoStackItem> getUndoStack() {
        return undoStack;
    }
    
    @Override
    public void setUndoStack()
    {
        this.undoStack = new Stack<>();
    }

    @Override
    public void saveGame(Game game, File file) {
        try {
            if(!(file.getName().endsWith(".kln")))
            {
                file = new File(file.toString() + ".kln");
            }
            FileOutputStream fos = new FileOutputStream(file);
            ObjectOutputStream oos = new ObjectOutputStream(fos);
            Stack<UndoStackItem> pom = this.undoStack;
            this.undoStack = null;
            oos.writeObject(game);
            oos.close();
            this.undoStack = pom;
            System.err.println("...saving game... done");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    @Override
    public Game loadGame(File file) {
        try {
            if (!file.exists()){
                //JOptionPane.showMessageDialog(this, "Subor neexistuje");
                throw new IOException("subor binsubor neexistuje");
            }

            FileInputStream fis = new FileInputStream(file);
            ObjectInputStream ois = new ObjectInputStream(fis);
            Game game = (Game) ois.readObject();
            ois.close();
            game.setUndoStack();
            System.err.println("...loading game... done");
            return game;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }


    /**
     * Inicializácia hry.
     * Rozloží karty na hrací plán a pripraví cieľové balíky.
     */
    @Override
    public void laidCards() {
        mainDeck = factory.createCardDeck();
        mainDeck.shuffleCards();

        for (Card.Color color : Card.Color.values()) {
            foundations.add(factory.createTargetPack(color));
        }

        for (int i = 0; i < 7; i++) {
            tableau.add(factory.createWorkingPack());
        }

        for (int i = 0; i < 7; i++) {                   // rozloženie kariet na kôpky
            for (int j = i; j < 7; j++) {
                CardStack stack = tableau.get(j);
                stack.put(mainDeck.get().color(), mainDeck.pop().value());
                if (i == j)
                    stack.get(i).turnFaceUp();
            }
        }
    }

    /**
     * Potiahne novú kartu zo zásobnika, ak sa tam nejaká nacházda, inak prevráti kôpku
     */
    @Override
    public boolean dealCard() {
        if (mainDeck.size() > 0) {
            waste.put(mainDeck.get().color(), mainDeck.pop().value());
            waste.get().turnFaceUp();
            return false;
        } else {
            int size = this.waste.size();
            for (int i = 0; i < size; i++) {
                mainDeck.put(waste.get().color(), waste.pop().value());
                mainDeck.get().turnFaceUp();
            }
            return true;
        }
    }

    /**
     * Zistí možný ťah v hre.
     */
    @Override
    public String getHint() {

        Card fromCard;
        Card toCard;
        String text;

        for (CardStack stack : tableau) {                                // beriem i-ty byliček
            for (int j = 0; j < stack.size(); j++) {
                fromCard = stack.get(j);                                // postupne beriem karty z i-teho balička

                if (fromCard.isTurnedFaceUp()) {
                    for (CardStack s : tableau) {                       // prejdenie tableau
                        if (s.checkPut(fromCard)) {
                            toCard = s.get();
                            text = ("Move " + fromCard.toString() + " to " + toCard.toString() + ".");         // todo - čo to bude vraciať ?
                            return text;
                        }
                    }
                    for (CardDeck d : foundations) {                    // prejdenie foundations
                        if (d.checkPut(fromCard)) {
                            //toCard = d.get();
                            text = ("Move " + fromCard.toString() + " to " +/* toCard.toString() +*/ "foundation.");         // todo - čo to bude vraciať ?
                            return text;
                        }}
                }
            }
        }
        if (waste.size() != 0) {
            fromCard = waste.get();
            for (CardStack st : tableau) {
                if (st.checkPut(fromCard)) {
                    toCard = st.get();
                    if (toCard == null && fromCard.value() == 13)
                        text = ("Move card from waste to empty stack.");
                    else
                        text = ("Move card from waste to " + toCard.toString() + ".");     // todo
                    return text;
                }
            }
            for (CardDeck deck : foundations) {
                if (deck.checkPut(fromCard)) {
                    text = ("Move card from waste to foundation");     // todo
                    return text;
                }
            }
        }
        if (mainDeck.size() != 0 || waste.size() != 0) {
            text = ("Deal card.");
            return text;// todo
        } else {
            text = ("No moves.");
            return text;// todo
        }

    }

    // todo
    @Override
    public void scoring(int from, int to) {

        if (from == WASTE) {
            if (to == TARGET) {
                addScore(10);
            }
            if (to == WORKING) {
                addScore(5);
            }
        } else if (from == WORKING && to == TARGET) {
            addScore(10);
        } else if (from == TARGET && to == WORKING) {
            addScore(-15);
        }
        // Waste to working	    ==   5
        // Waste to target	    ==  10
        // working to target	==  10
        // target to working	== −15
    }

    @Override
    public void scoring(int type) {
        // todo
        // Recycle waste when playing by ones	−100 (minimum score is 0)
        // Turn over Tableau card	==   5
        if (type == ROTATE) {
            addScore(5);
        } else if (type == RECYCLE) {
            addScore(-100);
        }
    }

    @Override
    public void addScore(int value) {
        int tmp = this.score;
        tmp += value;
        if (tmp < 0)
            this.score = 0;
        else
            this.score += value;
    }

    @Override
    public int getScore()
    {
        return this.score;
    }

    @Override
    public void setScore(int score) {
        this.score = score;
    }

    @Override
    public CardStack getStack(int index)
    {
        return this.tableau.get(index);
    }
    
    @Override
    public CardDeck getTarget(int index)
    {
        return this.foundations.get(index);
    }
    
    @Override
    public CardDeck getMainDeck()
    {
        return this.mainDeck;
    }

    @Override
    public CardDeck getWasteDeck()
    {
        return this.waste;
    }


}





