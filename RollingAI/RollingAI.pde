int framerate = 60; //<>// //<>// //<>// //<>// //<>//
long currentTime = 0;
int timestep = 1000/framerate;
int infoBoxHeight = 65;

Table[] tables;

int epoch = 1;
int genSize = 20;
int stillRunning = genSize;
int triesPerGen = 15;
int tryNr = 1;

int showOption = 0;
int showSpecificIndex = 0;
int shownIndex = -1;

float maxFitness = 0f;

void setup()
{
  size(500, 500);

  frameRate(framerate);
  smooth();

  textFont(createFont("Consolas", 10));

  initialize();
}

void initialize()
{
  tables = new Table[genSize];
  int data = checkRun();
  if (data == -1)
  {*/
    println("Starting new training session...");
    for (int i = 0; i < genSize; i++) 
      tables[i] = new Table(250, new PVector(width/2, height/2));
  /*} else {
    println("Continuing training session");
    InputStream input = createInput("Weights.txt");
    readMaxFit(input);
    readEpoch(input);

    tables[0] = new Table(250, new PVector(width/2, height/2), new FirstBrain(loadBrain(input)), maxFitness);
    try {
      input.read();
      for (int i = 1; i < genSize; i++) {
        tables[i] = new Table(250, new PVector(width/2, height/2), new FirstBrain(loadBrain(input)));
        input.read();
      }
    }
    catch(IOException e) {
      e.printStackTrace();
    }
  }
}

void mutate()
{
  epoch++;
  tryNr = 1;
  stillRunning = genSize;

  Table[] newTables = new Table[genSize];
  newTables[0] = getBestFit();
  maxFitness = newTables[0].fitness;
  if (epoch % 5 == 0)
    SaveWeights(tables, epoch, maxFitness);

  for (int i = 1; i < genSize - 1; i++)
  {
    Brain offspring = getParent().brain.getChild(getParent().brain);
    newTables[i] = new Table(250, new PVector(width/2, height/2), offspring);
  }

  for (int i = 1; i < genSize - 1; i++)
    newTables[i].brain.mutate(0.5, 0.3);

  newTables[genSize-1] = new Table(250, new PVector(width/2, height/2));

  tables = newTables;
  tables[0].reset();
  tables[0].fitness = 0;
}

Table getParent()
{
  float fitnessSum = 0f;
  for (int i = 0; i < genSize; i++)
    //if (tables[i].fitness > min(float(epoch)/10, 20))
    fitnessSum += tables[i].fitness;

  if (fitnessSum == 0)
    return tables[int(random(0, 19))];
  float probability = random(0f, fitnessSum);
  float runningSum = 0f;
  for (int i = 0; i < genSize; i++)
    //if (tables[i].fitness > min(float(epoch)/10, 20))
    {
      runningSum += tables[i].fitness;
      if (runningSum >= probability)
        return tables[i];
    }

  return null; // Should never get here
}

Table getBestFit()
{
  float max = max(tables[0].fitness, tables[0].allTimeBest);
  int maxIndex = 0;
  for (int i = 1; i < genSize; i++)
    if (max(tables[i].fitness, tables[i].allTimeBest) > max)
    {
      maxIndex = i;
      max = max(tables[i].fitness, tables[i].allTimeBest);
    }

  tables[maxIndex].fitness = tables[maxIndex].allTimeBest = max(tables[maxIndex].fitness, tables[maxIndex].allTimeBest);
  return tables[maxIndex];
}


void draw()
{
  currentTime += timestep;
  background(255, 204, 0);

  for (int i = 0; i < genSize; i++)
  {
    if (tables[i].isRunning())
      if (tables[i].update())
        stillRunning--;
  }

  if (stillRunning > 0)
    drawTable();
  else if (tryNr < triesPerGen)
  {
    tryNr++;
    for (int i = 0; i < genSize; i++)
      tables[i].reset();
    stillRunning = genSize;
  } else
  {
    mutate();
  }

  drawInfo();
}

void drawTable()
{
  if (showOption == 0) // First still running
  {
    int i;
    for (i = 0; i < genSize; i++)
      if (tables[i].isRunning())
      {
        tables[i].show();
        shownIndex = i;
        break;
      }

    if (i == genSize)
      shownIndex = -1;
  } else if (showOption == 1) // Highest fitness
  {
    float max = tables[0].fitness;
    for (int i = 1; i < genSize; i++)
      if (tables[i].fitness > max)
      {
        shownIndex = i;
        max = tables[shownIndex].fitness;
      }

    tables[shownIndex].show();
  } else if (showOption == 2) // Show specific
  {
  }
}

void drawInfo()
{
  fill(200);
  rect(-1, -1, width+1, infoBoxHeight);

  fill(0);
  text("FPS:     " + (int)frameRate, 5, 10);
  text("Running: " + stillRunning + "/" + genSize, 5, 20);
  text("Showing: " + shownIndex + "(" + (shownIndex != -1 ? tables[shownIndex].fitness : 0f) + ")", 5, 30);
  text("Epoch:   " + epoch, 5, 40);
  text("Max fit: " + maxFitness, 5, 50);
  text("Try nr.: " + tryNr + "/" + triesPerGen, 5, 60);
}

void mousePressed()
{
}

void SaveWeights(Table[] currentGen, int currentEpoch, float maxFitness)
{
  PrintWriter output = createWriter("Weights.txt");
  String weights = "" + maxFitness + "," + currentEpoch + "\n";
  FirstBrain brain;
  for (Table t : currentGen)
  {
    brain = (FirstBrain) t.brain;
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
    while (data != 44 && data >= 48 && data <= 57 )
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
  while (neuronCount < 6)
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
    while (data != 10 && data != 13 && neuronCount < 2)
    {
      data = loadNeuron(input, weights[neuronCount]);

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
      try{
        weights[i] = float(weight);
      }catch(ArrayIndexOutOfBoundsException e)
    {
      e.printStackTrace(); //<>//
    }
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
    e.printStackTrace();
  }
  return data;
}
