package text_adventure;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

public class TextAdventure {
    private static Room currentRoom;
    private static Map<String, Item> inventory = new HashMap<>();
    private static boolean isGameOver = false;
    private static Scanner scanner = new Scanner(System.in);
    private static boolean studyLocked = true;

    public static void main(String[] args) {
        initializeGame();
        
        System.out.println("Welcome to the Mysterious Manor!");
        System.out.println("You find yourself in an old, dusty mansion...fd");
        System.out.println("Type 'help' for a list of commands.\n");

        while (!isGameOver) {
            System.out.println("\n" + currentRoom.getDescription());
            System.out.print("> ");
            String input = scanner.nextLine().toLowerCase();
            processCommand(input);
        }
        
        scanner.close();
    }

    private static void initializeGame() {
        // Create rooms
        Room entrance = new Room("Entrance Hall", 
            "You're in a grand entrance hall. Dusty portraits line the walls. " +
            "A staircase leads upstairs, and doors lead east and west.");
        
        Room library = new Room("Entrance Hall", 
            "You're in a grand entrance hall. Dusty portraits line the walls. " +
            "A staircase leads upstairs, and doors lead east and west.");
        
        Room diningRoom = new Room("Entrance Hall", 
            "You're in a grand entrance hall. Dusty portraits line the walls. " +
            "A staircase leads upstairs, and doors lead east and west.");
        
        Room study = new Room("Entrance Hall", 
                "You're in a grand entrance hall. Dusty portraits line the walls. " +
                "A staircase leads upstairs, and doors lead east and west.");
        
        // Connect rooms
        entrance.addExit("east", library);
        entrance.addExit("west", diningRoom);
        entrance.addExit("upstairs", study);
        
        library.addExit("west", entrance);
        diningRoom.addExit("east", entrance);
        study.addExit("downstairs", entrance);

        // Add items to rooms with use actions

        Item book = new Item("A dusty book", "The title reads 'Ancient Mysteries'. A paper is sticking out.");
        book.setUseAction((location, target) -> {
            System.out.println("You open the book and find a note: 'The safe combination is 1842'");
            return true;
        });
        library.addItem("book", book);

        currentRoom = entrance;
    }

    private static void processCommand(String input) {
        String[] words = input.split(" ");
        String command = words[0];

        switch (command) {
            case "go":
                if (words.length < 2) {
                    System.out.println("Go where?");
                    return;
                }
                String direction = words[1];
                if (direction.equals("upstairs") && studyLocked) {
                    System.out.println("The door to the study is locked. You need to find a key.");
                    return;
                }
                Room nextRoom = currentRoom.getExit(direction);
                if (nextRoom != null) {
                    currentRoom = nextRoom;
                } else {
                    System.out.println("You can't go that way.");
                }
                break;

            case "look":
                System.out.println(currentRoom.getDetailedDescription());
                break;

            case "take":
                if (words.length < 2) {
                    System.out.println("Take what?");
                    return;
                }
                String itemName = words[1];
                Item item = currentRoom.removeItem(itemName);
                if (item != null && !item.getName().equals("A wall safe")) {
                    inventory.put(itemName, item);
                    System.out.println("Taken: " + item.getName());
                } else if (item != null) {
                    currentRoom.addItem(itemName, item);
                    System.out.println("You can't take that.");
                } else {
                    System.out.println("There's no " + itemName + " here.");
                }
                break;

            case "use":
                if (words.length < 2) {
                    System.out.println("Use what?");
                    return;
                }
                String itemToUse = words[1];
                String target = words.length > 3 && words[2].equals("on") ? words[3] : "";
                
                Item usedItem = inventory.get(itemToUse);
                if (usedItem != null) {
                    usedItem.use(currentRoom, target);
                } else {
                    Item roomItem = currentRoom.getItem(itemToUse);
                    if (roomItem != null) {
                        roomItem.use(currentRoom, target);
                    } else {
                        System.out.println("You don't have that item.");
                    }
                }
                break;

            case "inventory":
                if (inventory.isEmpty()) {
                    System.out.println("Your inventory is empty.");
                } else {
                    System.out.println("You are carrying:");
                    for (Item i : inventory.values()) {
                        System.out.println("- " + i.getName());
                    }
                }
                break;

            case "examine":
                if (words.length < 2) {
                    System.out.println("Examine what?");
                    return;
                }
                String targetName = words[1];
                Item targetItem = inventory.get(targetName);
                if (targetItem == null) {
                    targetItem = currentRoom.getItem(targetName);
                }
                if (targetItem != null) {
                    System.out.println(targetItem.getDescription());
                } else {
                    System.out.println("You don't see that here.");
                }
                break;

            case "help":
                System.out.println("Available commands:");
                System.out.println("- go [direction]: Move in a direction (e.g., 'go east')");
                System.out.println("- look: Look around the current room");
                System.out.println("- take [item]: Pick up an item");
                System.out.println("- use [item] (on [target]): Use an item, optionally on a target");
                System.out.println("- inventory: Check your inventory");
                System.out.println("- examine [item]: Examine an item in your inventory or room");
                System.out.println("- quit: Exit the game");
                break;

            case "quit":
                System.out.println("Thanks for playing!");
                isGameOver = true;
                break;

            default:
                System.out.println("I don't understand that command.");
                break;
        }
    }
}