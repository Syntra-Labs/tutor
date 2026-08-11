package text_adventure;

import java.util.HashMap;
import java.util.Map;

class Room {
    private String name;
    private String description;
    private Map<String, Room> exits;
    private Map<String, Item> items;

    public Room(String name, String description) {
        this.name = name;
        this.description = description;
        exits = new HashMap<>();
        items = new HashMap<>();
    }

    public String getName() {
        return name;
    }

    public void addExit(String direction, Room room) {
        exits.put(direction, room);
    }

    public Room getExit(String direction) {
        return exits.get(direction);
    }

    public void addItem(String name, Item item) {
        items.put(name, item);
    }

    public Item removeItem(String name) {
        return items.remove(name);
    }

    public Item getItem(String name) {
        return items.get(name);
    }

    public String getDescription() {
        return description;
    }

    public String getDetailedDescription() {
        StringBuilder sb = new StringBuilder(description + "\n");
        
        if (!exits.isEmpty()) {
            sb.append("\nExits: ");
            sb.append(String.join(", ", exits.keySet()));
        }
        
        if (!items.isEmpty()) {
            sb.append("\nItems here: ");
            sb.append(String.join(", ", items.keySet()));
        }
        
        return sb.toString();
    }
}