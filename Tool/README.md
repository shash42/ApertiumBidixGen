<h2>Requirements</h2>
C++17 or higher

<h2> Installation Guide</h2>
<ol>
<li> Download the Tool folder to your local machine.
<li>cd into the 'Codes' folder and run  
    g++ -std=c++17 Interface.cpp -lstdc++fs
<li> Enter ./a.out
<li> Follow the instructions given (more on this in the next section) and provide the required information to generate predictions!
<li> Check the Results folder and the corresponding experiment number's folder for the output of the algorithm.
</ol>

<h2> Usage Guidelines </h2>
After running the executable (./a.out for now), the program outputs the following: 

> Enter the Experiment Number:

It is important to understand why this is asked. The output of each run produces or overwrites data in a folder named with the experiment number you input. This folder is present inside Results/ (also produced by the program it doesn't already exist). So for example if you enter '2' then a folder with path Tool/Results/2/Analysis/ will contain the actions of the program. <br>
This essentially allows the user to run the program with separate config parameters and get results in separate folders to analyze what config parameters works best for them. <br>

If the folder number you input already exists, the program will re-confirm if you want to make changes to the folder (potentially overwriting the existing data). 

> Experiment 1 already exists. Are you sure you want to overwrite it? [Y/N]:

Enter 'Y' if that is okay, otherwise enter 'N' andyou will have to run the program again with the experiment number which you actually want to work on.

The next thing the program asks is:

> Do you want to generate new possibilities (1) or convert an existing possibilities file to predictions (2) ?:

Here, it is important to know that on running the main algorithm (option 1), it produces a 'possibilities.txt' file in the target experiment folder. These are translations accompanied by their 'confidence' scores. Then, the user can run this program again and convert the possibilities into predictions, produced in the 'predictions.txt' file. This is done to allow the user to play with different confidence thresholds and see which suits their needs best. So for example, if you enter '2', the following will be shown:

> Enter your required confidence threshold:

Say you enter 0.7 (or whatever other value). Then the following is requested:

> How many folders (inside the experiment's folder) do you want to generate predictions for?:

This is because the same experiment folder, multiple folders can exist for different language pairs, word translations etc. More on this later <br>
After inputting the required number, the following is requested:

> Enter 1 foldername per line

Simply put the name of each folder (inside the experiment folder) that you wish to generate predictions for, one per line (put in exactly the number of folders you specified in the previous prompt).

The program will now execute and generate the predictions.txt based on the possibilities.txt inside these folders. <br>
Example output:

> doll done! <br>
> en-ca done! <br>

Great! Now you've learnt how to generate predictions from possibilities, but how to generate possibilities in the first place? Let's go back to when the program asked for this:

> Do you want to generate new possibilities (1) or convert an existing possibilities file to predictions (2) ?:

Enter 1. The program now asks:

> Do you want to enter your own hyperparameter configurations? [Y/N]:

The program has some default hyperparameters, and these can be changed by you as the user to see what works best for your language-pair / requirements. If you enter Y, the program responds with:

> Enter Hyperparameter file path relative to Interface.cpp run location

The hyperparameters that you want should be available in a file. Suppose it's called "hyperparameters.txt" and it's stored in the folder right outside Codes (where Interface.cpp runs from). You then enter "../hyperparameters.txt". More on how this hyperparameter file can be configured later. <br>
If you want to continue with default hyperparameters, you can obviously just enter 'N'. Both options lead to the next prompt:

> Do you want to generate translations for some words (1) or get all translations for some languages (2) ?:

Essentially, the program provides 2 options. Either generate translations for some words or for some entire language pairs. The process further for both these processes is similar. Based on what you entered, you will get either of these two:

> Enter word, language list file path relative to run location

or

> Enter language list file path relative to run location

Provide the file just like the hyperparameters file above.  The config files required for the 2 options have to be formatted differently. More on that later. <br>
After this, the algorithm for generation will start running! It will provide some output like 'Loaded' within a minute which means that the input data has been read. You will then get outputs as different parts of your required output get processed (eg after each language-pair/word-translations are generated). It can take upto 10 minutes per language-pair generated normally.<br>

Once the program has terminated check the Results/ExperimentNumber/Analysis folder. It would have folders corresponding to the words/langauge-pairs you have produced translations for!

<h2> Config File Guidelines </h2>

There are 3 types of config files at present. a) hyperparameter file, b) language-pair file, c) word-file. Samples for all of the 3 types are provided in the download: "hyperparameters.txt", "langconfigfast.txt", "wordconfig.txt". <br>
As mentioned before, there are 2 main ways to generate new possibilities. One is a word-by-word approach where translations are found for a specified list of words. The other is a language pairs approach where results for multiple language-pairs can be generated (or as you will see, all possible predictions irrespective of language-pair). Both modes require a hyperparameters config file which is fed in earlier. Both modes have a separate type of config file described below.

<h3> Language-Pair Config file </h3>

A 'block' in a language-pair config file consists the follows: <br>
> Output Language Pair as "en es" or "NoFix integer" <br>
> Number of input language pairs, say M <br>
> SL~1~ SL~2~ SL~3~ ... SL~M~ <br>
> TL~1~ TL~2~ TL~3~ ... TL~M~ <br>

The first line contains the target language pair for which entries have to be generated. So for eg: "en es" means generating entries from english to spanish (and vice-versa). "en-es" will also become be the name of the folder in which results will be generated. Notably, it's also possible to not restrict the predicted translations' language pair by specifying the first line of the block as "NoFix 1" or "NoFix 2" or any other integer with "NoFix" (case-sensitive). In this case results will be produced in the "NoFix-1" or "NoFix-2" etc. folder. <br>
The second line contains the number of input language pairs, say N. This data will be used to produce predictions and the input data must be available in the LangData folder with file names like "Data-en-es.txt". The LangData folder is provided in installation. The 3rd and 4th line together contain the description of these N language pairs. The ith string on both lines forms a language pair. So for eg if line 3 and 4 are:

> en en fr <br>
> ca es ca <br>

This means the input language pairs are en-ca, en-es and fr-ca. Also, in this example the second line of the block must have 3. <br>

The first line of the language-pair config file consists of the number of blocks, say M. Description for M blocks follow in the following 4M lines. Example:

> Number of blocks, M <br>
> 4 LINES describing Block 1 <br>
> 4 LINES describing Block 2 <br>
> . <br>
> . <br>
> 4 LINES describing Block M <br>

Examples are provided in "langconfigfast.txt" and "languageconfig.txt", specifically the latter reproduces the experiment of removing 1 language pair and using the other 10 as input.

<h3> Word Config file </h3>
The brief format of the word config file is as follows:

> Output File Name <br>
> Number of words to find translations for, say N <br>
> Word 1 <br>
> Word 2 <br>
> .
> .
> Word N<br>
> Number of input language pairs, say M<br>
> SL~1~ SL~2~ SL~3~ ... SL~M~<br>
> TL~1~ TL~2~ TL~3~ ... TL~M~<br>

The output file name is where the final output results will be produced. The 2nd line contains the number of words for which the user needs translations. Then, one word per line needs to be added. Finally, the input language pair description which is used for generation follows much as explained above.
<h3> Hyperparameter Config file </h3>
First, it is important to understand the different kind of hyperparameters that can be tuned:
<h4> Understanding sets of Hyperparameters </h4>
<ol>
	<li> <b> Transitive </b> (alias: transitive) - Whether the translations should be produced in a transitive way or not. Actually this takes a non-binary value, specifically from 0-2. 0 means to use the proper cycle-algorithm. 1 means to use transitivity within the biconnected component (atleast 1 cycle must pass through source-word, target-word). 2 means a proper transitive relation. If this is set to 1 or 2, only the Context Depth hyperparameter is applicable, which defines the depth to which transitive relations are to be considered.
    <li> <b> Context Depth </b> (alias: context_depth) - Maximum length of shortest path from source word to target word <br>
    Increasing this: a) Increases Run-Time b) Increases |Pred| and precision (performance) till a point, after that becomes useless/reduces performance. Recommended range: 2-5. Default: 3 if transitive closure, 4 otherwise.
    <li> <b> Maximum Cycle Length </b> (alias: max_cycle_length) - Maximum length of cycle <br>
    Increasing this: a) Increases Run-Time b) Increases |Pred| and precision (performance) till a point, after that becomes redundant. Big dense cycles will have small denser subcycles. Recommended range 6-9. Default: 7
    <li> <b> Cutoff for being 'Large Context' </b> (alias: large_cutoff) - Minimum degree of source word for it to be 'Large Context' <br>
    Increasing means more contexts become small contexts and have lesser required minimum cycle length. If set to 0, all contexts are large context. Recommended range: 3-7, Default: 5
    <li> <b> Minimum Cycle Length in Large Contexts </b> (alias: large_min_cyc_len) - Minimum length of cycle in Large Contexts i.e. Deg(Source) greater or equal Cutoff <br>
    Increasing this: a) Decreases |Pred| b) Could slightly increase precision but don't make it too high/too low c) Slightly Decreases Run-Time. Recommend range: 4-6. Default: 5 
    <li> <b> Minimum Cycle Length in Small Contexts </b> (alias: small_min_cyc_len) - Minimum length of cycle in Small Contexts <br>
    Increasing this: Same as above. Recommend range: 4-5. Default: 4 
    <li> <b> Confidence Multiplier if Target has degree > 2 </b> (alias: deg_gt2_multiplier) - If the target has degree more than 2, confidence score is multiplied by this value <br>
     Increasing this: a) Increases |Pred| b) Might decrease Precision. Recommended Range: 1-1.5. Default: 1.3
    <li> <b> Confidence Threshold for predictions </b> (alias: conf_threshold) - Confidence Score cutoff for prediction to be be valid <br>
     Increasing this: a) Increases Precision b) Decreases |Pred|. Recommended Range: 0.5-0.8. Default: 0.65

In practice, tuning 1), 2), 3) and 8) should be more than sufficient. Confidence can be tuned after generating the 'possibilities' file (which contains all possible translations with confidence > 0) by using the possibilities to prediction feature.
 
 <h4> Different Hyperparameter Sets for Different POS </h4>
 It is possible to define different sets of hyperparameters for different parts of speech. By default, properNouns and numerals have a transitive POS, whereas others use cycle density algorithm defaults as described above. You can define multiple sets of hyperparameters in your hyperparameters file and then provide a mapping from POS to the hyperparameter set number as described in the file format below.

<h4> How to format the hyperparameters file </h4>
Right now, the formatting requirements for the hyperparameters file is quite strict. It is white-space sensitive, so please adhere to the format carefully. <br>
The formal structure of the file is as follows: <br>

> ~Hyperparameter Set~ 0 <br>
> end<br>
> ~Hyperparameter Set~ 1<br>
> end<br>
> .<br>
> .<br>
> .<br>
> POS_To_Hyperparameter_Map<br>
> POS1 = index-i<br>
> POS2 = index-j<br>
> .<br>
> .<br>
> .<br>
>ENDOFFILE<br>

~Hyperparameter Set~ as defined above is a group of lines of the following form:

> transitive = 0<br>
> context_depth = 4<br>
> max_cycle_length = 9<br>
> large_cutoff = 5<br>
> large_min_cyc_len = 5<br>
> small_min_cyc_len = 4<br>
> deg_gt2_multiplier = 1.1<br>
> conf_threshold = 0.65<br>

The given aliases are both case sensitive and any change in spelling would make them unrecognizable for the program. Note that it is not important to add specific values for all hyperparameters. If the value for a specific hyperparameter is not mentioned, it's default value will be used. So for example, cross-checking with the defaults above, the above set differs only in max_cycle_length and deg_gt2_multiplier. So it is sufficient to only specify them without any changes to the actual output/meaning:
> max_cycle_length = 9<br>
> deg_gt2_multiplier = 1.1<br>

A sample file ("hyperparameters.txt") is also provided to understand this paradigm. 

<hr> </hr>
In totality, it may be a bit overwhelming at once. Try to not play with the config files and use the ones provided in the install first. Using word-translations mode runs fast so use that initially. Then, start by making a new hyperparameter file and adding custom hyperparameters. Then customize the word-config / language-pair config and slowly it's easy to understand :)
