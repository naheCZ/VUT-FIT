package model.board;

import java.io.File;
import java.util.Stack;

import model.cards.CardDeck;
import model.cards.CardStack;

/**
 * project: ija_projekt
 * author: Adam Zivcak
 * login:  xzivca03
 * created: 31.3.2017
 */
public interface Game {

    Game getGame();

    CardStack getStack(int index);

    CardDeck getMainDeck();

    CardDeck getWasteDeck();

    Stack<UndoStackItem> getUndoStack();

    String getHint();

    boolean dealCard();

    void laidCards();

    void saveGame(Game game, File file);

    Game loadGame(File file);

    void pushItem(UndoStackItem i);

    UndoStackItem popItem();

    boolean checkWin();


    void scoring(int from, int to);

    void scoring(int type);

    int getScore();

    void setScore(int score);

    void addScore(int value);
    
    void setUndoStack();
    
    CardDeck getTarget(int index);

}