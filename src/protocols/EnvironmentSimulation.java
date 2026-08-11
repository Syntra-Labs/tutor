package protocols;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

//Main simulation controller
public class EnvironmentSimulation {
private List<Actor> actors;
private List<Sensor> sensors;
private EnvironmentPotentialField epf;
private EvaluationProcess evaluator;
private ActionsDesigner designer;

public EnvironmentSimulation() {
   this.actors = new ArrayList<>();
   this.sensors = new ArrayList<>();
   this.epf = new EnvironmentPotentialField();
   this.evaluator = new EvaluationProcess();
   this.designer = new ActionsDesigner();
   
   initializeSimulation();
}

private void initializeSimulation() {
   // Add initial actors
   actors.add(new Actor("Actor1", new Point(0, 0), new Goal("Reach target", 1.0)));
   actors.add(new Actor("Actor2", new Point(1, 1), new Goal("Maintain position", 0.8)));
   
   // Add sensors
   sensors.add(new Sensor("PositionSensor"));
   sensors.add(new Sensor("PotentialSensor"));
}

public void runSimulationStep() {
   // Calculate environment state
   EnvironmentState envState = epf.calculatePotentialField(actors);
   
   // Collect sensor data
   Map<String, Double> encodedState = new HashMap<>();
   for (Sensor sensor : sensors) {
       encodedState.putAll(sensor.encode(envState));
   }
   
   // Evaluate state for each actor
   for (Actor actor : actors) {
       EvaluationResult result = evaluator.evaluate(encodedState, actor.getGoal());
       List<String> suggestedActions = designer.designActions(result);
       
       // Log results
       System.out.println("Actor: " + actor.getGoal().getTargetValue());
       System.out.println("Evaluation Score: " + result.score);
       System.out.println("Suggested Actions: " + String.join(", ", suggestedActions));
       System.out.println();
   }
}

public static void main(String[] args) {
   EnvironmentSimulation sim = new EnvironmentSimulation();
   sim.runSimulationStep();
}
}