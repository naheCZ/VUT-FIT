package model.cards;

/**
 * projekt: Solitaire Klondike - IJA 2016/2017
 * autori: Roman Nahalka (xnahal01), Adam Zivcak (xzivca03)
 * dátum: 6.5.2017
 * <p>
 * Rozhranie pre prácu s CardStackom.
 */

public interface CardStack extends CardDeck {

    /**
     * Metóda odebere ze zásobníka sekvenciu kariet od zadanej karty až po vrchol zásobníka.
     * Ak je hľadaná karta na vrchole, bude v sekvencii len jedna karta.
     *
     * @param card Hľadaná karta.
     * @return Zásobník kariet obsahujúci odobranú sekvenciu. Ak hľadaná karta v zásobníku nie je, vracia null.
     */
    CardStack pop(Card card);

    /**
     * Vloží karty zo zásobníka stack na vrchol zásobníka.
     * Karty vkladá v rovnakom poradí, v akom sú uvedené v zásobníku stack.
     *
     * @param stack Zásobník vkládaných kariet.
     * @return Uspešnosť akcie.
     */
    boolean put(CardStack stack);

    /**
     * Overí či je možné zadanú kartu položiť na balíček kariet.
     *
     * @param card Vkladaná karta
     * @return Úspešnosť akcie - true, ak je položenie možné.
     */
    boolean checkPut(Card card);

    /**
     * Otočí poslednú kartu v balíčku.
     *
     * @return True ak sa otočenie vykonalo.
     */
    boolean rotateLastCard();

}