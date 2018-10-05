package graphics;

import model.board.Game;
import model.cards.CardStack;
/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 *
 * Rozhranie pre pracu s balickami typu stack.
 */
public interface StackPanelInterface {

    /**
     * Nastavi prirazeny herni balicek a hru, kte ktere se vaze a pote tento
     * balicek vykresli.
     * @param stack Zasobnik karet, prirazeny k tomuto objektu.
     * @param game Hra, ke ktere tento hraci balicek patri.
     */
    void setStack(CardStack stack, Game game);

    /**
     * Vrati zasobnik karet, ktery je prirazen k tomuto objektu.
     * @return Zasobnik karet
     */
    CardStack getStack();
}
