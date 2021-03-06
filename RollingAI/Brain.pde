interface Brain
{
  float[] percieve(float... input);

  Brain getChild(Brain with);
  void mutate(float rate, float probability);
}

class FirstBrain implements Brain
{
  Neuron[] input = new Neuron[] { new Neuron(), new Neuron(), new Neuron(), new Neuron(), new Neuron(), new Neuron() };
  Neuron[] hl1;
  Neuron[] output;

  int inputLength = input.length;
  int hl1Length, outputLength;

  FirstBrain()
  {
    Linear linear = new Linear();
    Sigmoid sigmoid = new Sigmoid();
    Tipping tipping = new Tipping();
    ReLU relu = new ReLU();
    Composite tipInv = new Composite(tipping, new Composite(sigmoid, new Inverse(-30f)));
    Composite resig = new Composite(sigmoid, relu);

    hl1 = new Neuron[] {
      new Neuron(resig, input), //Reason for up
      new Neuron(resig, input), //Reason for up
      new Neuron(resig, input), //Reason for up
      new Neuron(resig, input), //Reason for up
      new Neuron(tipInv, input), 
      new Neuron(tipInv, input), 
    };

    output = new Neuron[] {
      new Neuron(tipping, hl1), 
      new Neuron(tipping, hl1), 
      new Neuron(tipping, hl1), //
      new Neuron(tipping, hl1), //intsY
    };

    hl1Length = hl1.length;
    outputLength = output.length;
  }

  FirstBrain(float[][][] weights)
  {
    Linear linear = new Linear();
    Sigmoid sigmoid = new Sigmoid();
    Tipping tipping = new Tipping();
    ReLU relu = new ReLU();
    Composite tipInv = new Composite(tipping, new Composite(sigmoid, new Inverse(-30f)));
    Composite resig = new Composite(sigmoid, relu);


    hl1 = new Neuron[] {
      new Neuron(linear, input, weights[0][0]), 
      new Neuron(linear, input, weights[0][1]), 
      new Neuron(linear, input, weights[0][2]), 
      new Neuron(linear, input, weights[0][3]), 
      new Neuron(tipInv, input, weights[0][4]), 
      new Neuron(tipInv, input, weights[0][5]), 
    };

    output = new Neuron[] {
      new Neuron(tipping, hl1, weights[1][0]), 
      new Neuron(tipping, hl1, weights[1][1]), 
      new Neuron(tipping, hl1, weights[1][2]), 
      new Neuron(tipping, hl1, weights[1][3]), 
    };

    hl1Length = hl1.length;
    outputLength = output.length;
  }


  float[] percieve(float... _input)
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

    return result;
  }

  Brain getChild(Brain _with)
  {
    FirstBrain newBrain = new FirstBrain();
    FirstBrain with = (FirstBrain)_with;

    for (int i = 0; i < hl1Length; i++)
      for (int x = 0; x < inputLength; x++)
        newBrain.hl1[i].weights[x] = round(random(1)) == 0 ? hl1[i].weights[x] : with.hl1[i].weights[x];

    for (int i = 0; i < outputLength; i++)
      for (int x = 0; x < hl1Length; x++)
        newBrain.output[i].weights[x] = round(random(1)) == 0 ? output[i].weights[x] : with.output[i].weights[x];

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

  Neuron()
  {
  }

  Neuron(ActivationFunction _func, Neuron[] _inputs)
  {
    this(_func, _inputs, new float[_inputs.length], 1f);

    inputCount = inputs.length;
    for (int i = 0; i < inputCount; i++)
      weights[i] = random(2) - 1;
  }

  Neuron(ActivationFunction _func, Neuron[] _inputs, float[] _weights, float _bias)
  {
    func = _func;
    inputs = _inputs;
    weights = _weights;
    bias = _bias;
  }

  Neuron(ActivationFunction _func, Neuron[] _inputs, float[] _weights)
  {
    func = _func;
    inputs = _inputs;
    int wl = _weights.length;
    float[] w = new float[wl-1];
    for (int i = 0; i < wl - 1; i++)
      w[i] = _weights[i + 1];
    weights = w;
    bias = _weights[0];
  }


  void compute()
  {
    float sum = bias;
    for (int i = 0; i < inputCount; i++)
      sum += weights[i] * inputs[i].output;

    output = func.compute(sum);
  }

  void mutate(float rate, float probability)
  {
    if (random(1) < probability)
      bias += random(2*rate) - rate;

    for (int i = 0; i < inputCount; i++)
      if (random(1) < probability)
        weights[i] += random(2*rate) - rate;
  }
}
