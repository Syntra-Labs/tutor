package methodology;

import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

class EpistemicGame {
    private Scenario scenario;
    private Map<Hypothesis, Double> beliefs;
    private int turn;
    private boolean gameOver;
    
    public void initialize(Scenario scenario) {
        this.scenario = scenario;
        this.turn = 1;
        this.gameOver = false;
        initializeBeliefs();
    }
    
    private void initializeBeliefs() {
        beliefs = new HashMap<>();
        double totalPrior = scenario.getHypotheses().stream()
            .mapToDouble(Hypothesis::getPrior)
            .sum();
        
        for(Hypothesis h : scenario.getHypotheses()) {
            beliefs.put(h, h.getPrior() / totalPrior);
        }
    }
    
    public void processTurn(List<Action> actions) {
        if(gameOver) return;
        
        System.out.println("\n--- Turn " + turn + " ---");
        actions.forEach(this::processAction);
        updateBeliefs();
        checkGameEnd();
        turn++;
    }
    
    private void processAction(Action action) {
        switch(action.getType()) {
            case OBSERVE:
                System.out.println("Observing: " + action.getDescription());
                updateLikelihoods(action.getAffectedHypotheses(), action.getLikelihoodImpact());
                break;
            case TEST:
                System.out.println("Testing: " + action.getDescription());
                applyTestResults(action.getAffectedHypotheses(), action.getSuccess());
                break;
            case QUESTION:
                System.out.println("Questioning: " + action.getDescription());
                break;
            case FALSIFY:
                System.out.println("Attempting falsification: " + action.getDescription());
                applyFalsification(action.getAffectedHypotheses());
                break;
        }
    }
    
    private void updateLikelihoods(List<Hypothesis> hypotheses, double multiplier) {
        hypotheses.forEach(h -> h.updateLikelihood(multiplier));
    }
    
    private void applyTestResults(List<Hypothesis> hypotheses, boolean success) {
        double adjustment = success ? 1.2 : 0.8;
        hypotheses.forEach(h -> h.updateLikelihood(adjustment));
    }
    
    private void applyFalsification(List<Hypothesis> hypotheses) {
        hypotheses.forEach(h -> h.updateLikelihood(0.5));
    }
    
    private void updateBeliefs() {
        double totalEvidence = scenario.getHypotheses().stream()
            .mapToDouble(h -> h.getPrior() * h.getLikelihood())
            .sum();
        
        for(Hypothesis h : scenario.getHypotheses()) {
            double newBelief = h.getPosterior(totalEvidence);
            beliefs.put(h, newBelief);
            h.setPrior(newBelief); // Update prior for next round
            h.updateLikelihood(1.0); // Reset likelihood
        }
        
        printBeliefs();
    }
    
    private void checkGameEnd() {
        Optional<Map.Entry<Hypothesis, Double>> maxEntry = beliefs.entrySet().stream()
            .max(Map.Entry.comparingByValue());
        
        if(maxEntry.isPresent() && maxEntry.get().getValue() > 0.95) {
            System.out.println("\nGame Over! Conclusion: " + maxEntry.get().getKey().getName());
            if(maxEntry.get().getKey().getName().equals(scenario.getHiddenTruth())) {
                System.out.println("Correct truth identified!");
            } else {
                System.out.println("Incorrect conclusion! Hidden truth was: " + scenario.getHiddenTruth());
            }
            gameOver = true;
        }
    }
    
    private void printBeliefs() {
        System.out.println("Current beliefs:");
        beliefs.entrySet().stream()
            .sorted(Map.Entry.comparingByValue(Comparator.reverseOrder()))
            .forEach(e -> System.out.printf("- %s: %.2f%%%n", 
                e.getKey().getName(), 
                e.getValue() * 100));
    }
}