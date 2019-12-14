int framerate = 60;
int timestep = 1000/framerate;
long currentTime = 0;

boolean runAI = true;
boolean trainAI = false;

int epoch = 1;
int genSize = 500;
int stillRunning;
int triesPerEpoch = trainAI ? 5 : 1;
int tryNr = 1;

float maxFitness = 0;
float mutationRate = 0.05;
float mutationProbability = 0.15;

PVector tablePos = new PVector(200, 250);
Table[] tables;

Table bestTable, firstAlive;
boolean showBestElseFirstAlive = false;

PrintWriter writer;


void setup()
{
  size(400, 400);
  textFont(createFont("Consolas",10));
  
  frameRate(framerate);
  smooth();
  
  initialize();
}

void initialize()
{
  if (runAI && trainAI)
  {
    tables = new Table[genSize];
    
    int data = checkRun();
    if (data == -1)
    {
      println("Starting new training session...");
      for (int i = 0; i < genSize; i++)
        tables[i] = new Table();
    } 
    else 
    {
      println("Continuing training session");
      InputStream input = createInput("Weights.txt");
      readMaxFit(input);
      readEpoch(input);
  
      tables[0] = new Table(new AI(loadBrain(input)), maxFitness);
      try {
        input.read();
        for (int i = 1; i < genSize; i++) {
          tables[i] = new Table(new AI(loadBrain(input)));
          input.read();
        }
      }
      catch(IOException e) {
        e.printStackTrace();
      }
    }
  }
  else if (!runAI)
  {
    String[] lines = loadStrings("PDfit.txt");
    writer = createWriter("PDfit.txt");
    for (int i = 0; i < lines.length; i++) {
      writer.println(lines[i]);  
    }
    
    tables = new Table[] { new Table() };
    tables[0].brain = new PD();
  }
  else
  {
    String[] lines = loadStrings("AIfit.txt");
    writer = createWriter("AIfit.txt");
    for (int i = 0; i < lines.length; i++) {
      writer.println(lines[i]);  
    }
    
    float[][][] weights = new float[][][] 
    {
      {  
        {0f, -0.26396087, 0.17738606,-1.04795900, 0.30522442},
        {0f, -0.09836733, 0.16733426, 0.85635210, 1.41451500},
        
        {0f, -0.06680263, 0.01970565,-0.15914561,-0.05600453},
        {0f,  0.37515295,-0.08730203,-0.49338546,-0.56916930},
      },
      {
        {0f, -0.37129474, 0.36047890, 0.20418686, 0.61584735},
        {0f,  0.03696144, 0.51748920,-0.10813793, 0.16429268},
      },
    };
    
    tables = new Table[] { new Table() };
    tables[0].brain = new AI(weights);
  }
  
  stillRunning = runAI && trainAI ? genSize : 1;
  bestTable = tables[0];
}


void draw()
{
  currentTime += timestep;
  background(255, 204, 0);
  
  update();
  
  if (stillRunning > 0)
    (showBestElseFirstAlive ? bestTable : firstAlive).show(tablePos);
  else if (tryNr < triesPerEpoch)
  {
    tryNr++;
    stillRunning = runAI && trainAI ? genSize : 1;
    for (Table t : tables) 
      t.reset();
  }
  else
    nextEpoch();
    
  drawInfo();
}


void update()
{
  boolean foundFirst = false;
  for (Table t : tables)
  {
    if (!t.isRunning())
      continue;
      
    if (!foundFirst)
    {
      firstAlive = t;
      foundFirst = true;
    }
    
    t.update();
    if (t.fitness > bestTable.fitness)
      bestTable = t;
      
    if (!t.isRunning())
      stillRunning--;
  }
}

void nextEpoch()
{
  epoch++;
  tryNr = 1;
  stillRunning = runAI && trainAI ? genSize : 1;
 
  if (runAI && trainAI)
  {
    for (Table t : tables)
      if (t.fitness > maxFitness)
        maxFitness = t.fitness;
        
    mutate();
  }
  else
  {
    writer.println(bestTable.fitness);
    writer.flush();
    bestTable.reset();
    bestTable.fitness = 0;
  }
}

void mutate()
{
  Table[] newTables = new Table[genSize];
  
  newTables[0] = getBestFit();
  for (int i = 1; i < genSize - 50; i++)
  {
    newTables[i] = new Table(getParent().getChild(getParent()));
    ((AI)newTables[i].brain).mutate(mutationRate, mutationProbability);
  }
  newTables[0].fitness = 0;
  newTables[0].reset();
  
  for (int i = genSize - 50; i < genSize; i++)
    newTables[i] = new Table();
  
  tables = newTables;

  if (epoch % 5 == 0)
  {
    SaveWeights(tables, epoch, maxFitness);
    SaveWeightsToFolder(tables, epoch, maxFitness);
  }
}

Table getBestFit()
{
  float max = max(tables[0].fitness, tables[0].allTimeBestFit);
  int maxIndex = 0;
  for (int i = 1; i < genSize; i++)
    if (max(tables[i].fitness, tables[i].allTimeBestFit) > max)
    {
      maxIndex = i;
      max = max(tables[i].fitness, tables[i].allTimeBestFit);
    }

  tables[maxIndex].allTimeBestFit = tables[maxIndex].allTimeBestFit = max(tables[maxIndex].fitness, tables[maxIndex].allTimeBestFit);
  return tables[maxIndex];
}

AI getParent()
{
  float fitnessSum = 0f;
  for (Table t : tables)
    fitnessSum += t.fitness;
    
  if (fitnessSum == 0)
    return (AI)tables[int(random(0, 19))].brain;
    
  float probability = random(0f, fitnessSum);
  float runningSum = 0f;
  for (Table t : tables)
  {
    runningSum += t.fitness;
    if (runningSum >= probability)
      return (AI)t.brain;
  }

  return null; // Should never get here
}


void drawInfo()
{
  fill(200);
  rect(-1, -1, width+1, runAI && trainAI ? 65 : 35);
  fill(0);
  
  if (runAI && trainAI)
  {
    text("Epoch:     " + epoch, 5, 10);
    text("Try nr.:   " + tryNr + "/" + triesPerEpoch, 5, 30);
    text("Running:   " + stillRunning + "/" + genSize, 5, 20);
    text("Max fit:   " + maxFitness, 5, 40);
    text("Fitness:   " + (showBestElseFirstAlive ? bestTable : firstAlive).fitness, 5, 50);
    text("Live-time: " + (showBestElseFirstAlive ? bestTable : firstAlive).aliveTime, 5, 60);
  }
  else
  {
    text("Variant:   " + (runAI ? "AI" : "PD"), 5, 10);
    text("Fitness:   " + bestTable.fitness, 5, 20);
    text("Live-time: " + bestTable.aliveTime, 5, 30);
  }
}



// ========================================================================================== //



void SaveWeights(Table[] currentGen, int currentEpoch, float maxFitness)
{
  PrintWriter output = createWriter("Weights.txt");
  String weights = "" + maxFitness + "," + currentEpoch + "\n";
  AI brain;
  for (Table t : currentGen)
  {
    brain = (AI)t.brain;
    for (Neuron n : brain.hl1)
    {
      weights += n.bias;
      for (float f : n.weights)
      {
        weights += "," + f;
      }
      weights += "! ";
    }
    weights += "#";
    for (Neuron n : brain.output)
    {
      weights += n.bias;
      for (float f : n.weights)
      {
        weights +="," + f;
      }
      weights += "! ";
    }
    weights += "\n";
  }
  output.write(weights);
  output.flush();
  output.close();
}

void readMaxFit(InputStream input)
{
  try {
    String maxFitString = "";
    int data = input.read();
    while (data != 44 && data >= 46 && data <= 57)
    {
      maxFitString += char(data);
      data = input.read();
    }
    maxFitness = float(maxFitString);
  }
  catch(IOException e) {
    e.printStackTrace();
  }
}

void readEpoch(InputStream input)
{
  try {
    String epochString = "";
    int data = input.read();
    while (data >= 48 && data <= 57)
    {
      epochString += char(data);
      data = input.read();
    }
    epoch = int(epochString);
  }
  catch(IOException e) {
    e.printStackTrace();
  }
}

float[][][] loadBrain(InputStream input)
{
  float[][][] weights = new float[2][6][7];
  loadHiddenLayer(input, weights[0]);
  try {
    input.read();
    input.read();
  }
  catch(IOException e) {
    e.printStackTrace();
  }
  loadOutputLayer(input, weights[1]);
  return weights;
}

void keyPressed() {
  keyPressed = true;
}

void loadHiddenLayer(InputStream input, float[][] weights)
{
  int neuronCount = 0;
  while (neuronCount < 4)
  {
    loadNeuron(input, weights[neuronCount]);
    neuronCount++;
  }
}

void loadOutputLayer(InputStream input, float[][] weights)
{
  int data = 0;
  try {
    int neuronCount = 0;
    while (data != 10 && data != 13 && neuronCount < 2) //<>//
    { //<>//
      data = loadNeuron(input, weights[neuronCount]); //<>//
      
      data = input.read();
      neuronCount++;
    }
  }
  catch(IOException e)
  {
    e.printStackTrace();
  }
}

int loadNeuron(InputStream input, float[] weights)
{
  int data = 0;
  String weight = "";
  try {
    data = input.read();
    if (data == 32)
      data = input.read();
    if (data == 35 || data == 10 || data == 13)
      return data;
    while (data == 46 || (48 <= data && data <= 57) || data == 45 || data == 69) {
      weight += char(data);
      data = input.read();
    }
    weights[0] = float(weight);
    weight = "";
    int i = 1;
    while (data != 33) {
      data = input.read();
      while (data == 46 || (48 <= data && data <= 57) || data == 45 || data == 69) {
        weight += char(data);
        data = input.read();
      }
      try {
        weights[i] = float(weight);
      } //<>//
      catch(ArrayIndexOutOfBoundsException e) //<>//
      { //<>// //<>//
        e.printStackTrace(); //<>//
      } //<>//
      weight = "";
      i++;
    }
  }
  catch(IOException e) {
    e.printStackTrace();
  }
  return data;
}

int checkRun()
{
  InputStream input = createInput("Weights.txt");
  int data = 0;
  try {
    data = input.read();
    input.close();
  }
  catch(IOException e) {
    e.printStackTrace(); //<>//
  } //<>//
  return data; //<>// //<>//
} //<>//
 //<>//
void SaveWeightsToFolder(Table[] currentGen, int currentEpoch, float maxFitness)
{
  String file = "weights\\weights" + epoch/5 + ".txt";
  PrintWriter output = createWriter(file);
  String weights = "" + maxFitness + "," + currentEpoch + "\n";
  AI brain;
  for (Table t : currentGen)
  {
    brain = (AI)t.brain;
    for (Neuron n : brain.hl1)
    {

      weights += n.bias;
      for (float f : n.weights)
      {
        weights += "," + f;
      }
      weights += "! ";
    }
    weights += "#";
    for (Neuron n : brain.output)
    {
      weights += n.bias;
      for (float f : n.weights)
      {
        weights +="," + f;
      }
      weights += "! ";
    }
    weights += "\n";
  }
  output.write(weights);
  output.flush();
  output.close();
}
