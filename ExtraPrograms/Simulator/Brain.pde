interface Brain
{
  PVector compute(float ... _input);
}

class PD implements Brain
{
  //values for PID
  double innerInput, outerInput;
  double innerOutput, outerOutput;
  double period = 184;
  double integralSumInner = 0, integralSumOuter = 0;
  double lastInputInner = 0, lastInputOuter = 0;
  double outputInner = 0, outputOuter = 0;
  int sameCount = 0;
  double curX = 0, curY = 0, prevX = 0, prevY = 0;
  
  //tuning values for PID
  double IKp = 0.025, OKp = 0.025, IKi = 0, OKi = 0, IKd = 30, OKd = 30;
  
  PVector compute(float ... _input)
  {
    curX = _input[0];
    curY = _input[1];
    
    double proportionalInner = IKp * curX;
    double integralInner     = integralSumInner + IKi * curX * period;
    double derivativeInner   = IKd * (curX - lastInputInner) / period;

    double proportionalOuter = OKp * curY;
    double integralOuter     = integralSumOuter + OKi * curY * period;
    double derivativeOuter   = OKd * (curY - lastInputOuter) / period;

    integralSumInner = integralInner;
    integralSumOuter = integralOuter;

    lastInputInner = curX;
    lastInputOuter = curY;

    outputInner = proportionalInner + integralInner + derivativeInner;
    outputOuter = proportionalOuter + integralOuter + derivativeOuter;
    
    return new PVector((float)outputInner, (float)outputOuter); //<>//
  }
}

// ================================================================================================================================

Tilt tilt = new Tilt();
Edge edge = new Edge();
Cntr cntr = new Cntr();

    
class AI implements Brain
{
  Neuron[] input = new Neuron[] { new Neuron(), new Neuron(), new Neuron(), new Neuron() };
  Neuron[] hl1;
  Neuron[] output;

  int inputLength = input.length;
  int hl1Length, outputLength;
  
  
  AI()
  {
    hl1 = new Neuron[] {
      new Neuron(cntr, input),
      new Neuron(cntr, input),
      new Neuron(edge, input),
      new Neuron(edge, input),
    };

    output = new Neuron[] {
      new Neuron(tilt, hl1), 
      new Neuron(tilt, hl1), 
    };

    hl1Length = hl1.length;
    outputLength = output.length;
  }

  AI(float[][][] weights)
  {
    hl1 = new Neuron[] {
      new Neuron(cntr, input, weights[0][0]),
      new Neuron(cntr, input, weights[0][1]),
      new Neuron(edge, input, weights[0][2]),
      new Neuron(edge, input, weights[0][3]),
    };

    output = new Neuron[] {
      new Neuron(tilt, hl1, weights[1][0]), 
      new Neuron(tilt, hl1, weights[1][1]), 
    };

    hl1Length = hl1.length;
    outputLength = output.length;
  }
  
  
  PVector compute(float ... _input)
  {
    for (int i = 0; i < inputLength; i++)
      input[i].output = _input[i];

    for (int i = 0; i < hl1Length; i++)
      hl1[i].compute();

    for (int i = 0; i < outputLength; i++)
      output[i].compute();

    float[] result = new float[outputLength];
    for (int i = 0; i < outputLength; i++)
      result[i] = output[i].output;

    return new PVector(result[0], result[1]);
  }
  
  
  AI getChild(AI ai)
  {
    AI newBrain = new AI();

    for (int i = 0; i < hl1Length; i++)
      for (int x = 0; x < inputLength; x++)
        newBrain.hl1[i].weights[x] = random(1) < 0.5 ? hl1[i].weights[x] : ai.hl1[i].weights[x];

    for (int i = 0; i < outputLength; i++)
      for (int x = 0; x < hl1Length; x++)
        newBrain.output[i].weights[x] = random(1) < 0.5 ? output[i].weights[x] : ai.output[i].weights[x];

    return newBrain;
  }

  void mutate(float rate, float probability)
  {
    for (Neuron n : hl1)
      n.mutate(rate, probability);

    for (Neuron n : output)
      n.mutate(rate, probability);
  }
}

// ---------------------------------------------------------------------------------

class Neuron
{
  ActivationFunction func;
  Neuron[] inputs;
  float[] weights;
  float bias;
  float output;

  int inputCount;

  Neuron() { }

  Neuron(ActivationFunction _func, Neuron[] _inputs)
  {
    this(_func, _inputs, new float[_inputs.length], 0f);

    inputCount = inputs.length;
    for (int i = 0; i < inputCount; i++)
      weights[i] = random(0.4) + 0.3;
  }

  Neuron(ActivationFunction _func, Neuron[] _inputs, float[] _weights, float _bias)
  {
    this.func = _func;
    this.inputs = _inputs;
    this.weights = _weights;
    this.bias = _bias;
    inputCount = inputs.length;
  }

  Neuron(ActivationFunction _func, Neuron[] _inputs, float[] _weights)
  {
    this.func = _func;
    this.inputs = _inputs;
    int wl = _inputs.length;
    float[] w = new float[wl];
    for (int i = 0; i < wl; i++)
      w[i] = _weights[i + 1];
    this.weights = w;
    this.bias = _weights[0];
    inputCount = inputs.length;
  }


  void compute()
  {
    float sum = bias;
    for (int i = 0; i < inputCount; i++)
      if (abs(weights[i]) >= 0.00001 && abs(inputs[i].output) >= 0.00001)
        sum += weights[i] * inputs[i].output;

    output = func.compute(sum);
  }

  void mutate(float rate, float probability)
  {
    //if (random(1) < probability)
    //  bias += random(2*rate) - rate;

    for (int i = 0; i < inputCount; i++)
      if (random(1) < probability)
        weights[i] += random(2*rate) - rate;
  }
}
