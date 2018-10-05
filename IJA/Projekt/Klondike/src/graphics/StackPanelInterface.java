package graphics;

import model.board.Game;
import model.cards.CardStack;

/**
 * project: Klondike (copy)
 * author: Adam Zivcak
 * login:  xzivca03
 * created: 8.4.2017
 */
public interface StackPanelInterface {

    void setStack(CardStack stack, Game game);

    CardStack getStack();
}
