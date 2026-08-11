package text_adventure;

@FunctionalInterface
interface ItemUseAction {
    boolean perform(Room location, String target);
}