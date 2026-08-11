package methodology;

import java.util.Arrays;
import java.util.List;

//Example usage
public class EpistemicGameEngine {
 public static void main(String[] args) {
     // Configure a scenario
     Hypothesis smoking = new Hypothesis("Smoking causes cancer", 0.4);
     Hypothesis pollution = new Hypothesis("Pollution causes cancer", 0.3);
     Hypothesis genetics = new Hypothesis("Genetics causes cancer", 0.3);
     
     Scenario cancerScenario = new Scenario(
         "Cancer Causation Investigation",
         "Smoking causes cancer",
         Arrays.asList(smoking, pollution, genetics)
     );
     
     // Initialize game
     EpistemicGame game = new EpistemicGame();
     game.initialize(cancerScenario);
     
     // Create actions
     List<Action> turn1Actions = Arrays.asList(
         new Action(ActionType.OBSERVE, "Observe cancer rates in smokers", 
                   List.of(smoking), 2.0, null),
         new Action(ActionType.QUESTION, "Are smokers also exposed to pollution?", 
                   List.of(smoking, pollution), null, null)
     );
     
     List<Action> turn2Actions = Arrays.asList(
         new Action(ActionType.TEST, "Controlled smoking study", 
                   List.of(smoking), null, true),
         new Action(ActionType.FALSIFY, "Look for non-smoking cancer cases", 
                   List.of(smoking), null, null)
     );
     
     // Play turns
     game.processTurn(turn1Actions);
     game.processTurn(turn2Actions);
 }
}