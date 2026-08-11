import java.util.Scanner;

public class Game {
	private Player player;
	private Location currentLocation;
	
	public Game(String playerName) {
		player = new Player(playerName);
		setupLocations();
	}
	
	private void setupLocations() {
        Location forest = new Location("You are in a dark forest. You can see a path leading north.");
        forest.addAction("Go north");
        forest.addAction("Pick up a stick");

        currentLocation = forest; // Start in the forest
    }

    public void start() {
        Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.println(currentLocation.getDescription());
            System.out.println("What would you like to do?");
            for (int i = 0; i < currentLocation.getActions().size(); i++) {
                System.out.println((i + 1) + ". " + currentLocation.getActions().get(i));
            }
            int choice = scanner.nextInt();
            processChoice(choice);
        }
    }

    private void processChoice(int choice) {
        String action = currentLocation.getActions().get(choice - 1);
        switch (action) {
            case "Go north":
                System.out.println("You walk north into the unknown.");
                break;
            case "Pick up a stick":
                player.addItem("Stick");
                System.out.println("You picked up a stick.");
                break;
            default:
                System.out.println("Invalid choice.");
        }
    }

	public static void main(String[] args) {
		Game game = new Game("Hero");
		game.start();

	}

}
