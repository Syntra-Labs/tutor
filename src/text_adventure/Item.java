package text_adventure;

class Item {
    private String name;
    private String description;
    private ItemUseAction useAction;

    public Item(String name, String description) {
        this.name = name;
        this.description = description;
        this.useAction = (location, target) -> {
            System.out.println("You can't use this item that way.");
            return false;
        };
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public void setUseAction(ItemUseAction action) {
        this.useAction = action;
    }

    public boolean use(Room location, String target) {
        return useAction.perform(location, target);
    }
}
