<h2>Requirements</h2>
C++17 or higher

<h2> Installation Guide</h2>
<ol>
<li> Download the Tool folder to your local machine.
<li> cd into the 'Codes' folder
<li> Parse the bidixes provided in the download by the commands <code> g++ -o parse -std=c++17 BidixParsing.cpp -lstdc++fs </code> and then <code> ./parse config_file_path </code>. Described in more detail below.
<li> Use following the usage guidelines below. </li>
</ol>

<h2> Parsing Bilingual Dictionaries or RDF Data </h2>

The algorithm reads from a simple internal format different from both Apertium Bilingual Dictionaries and RDF Data. This is so that in the future if data from more sources needs to be taken, they can be converted into this simple format by writing only a new parsing script. After that, the algorithm can use this data too. <br>

Typically, you will have to parse the provided directories once after installation. In the future you can reuse that parsed data. If you add any new bidixes, you'll have to parse them too. <br>

First, it's important to describe the lang-file-list. This is what is passed to both for Apertium bidix format and RDF CSV parsing. It's an easy format for other parsers too. It consists of multiple "blocks" where one block provides the data for one language pair. It is defined as follows: <br>

> Path <br>
>
> LeftLang code RightLang code <br>

Path describes the path to the raw Apertium bidix file. The 2 "Lang code"s are the language codes of the languages the bidix file contains. For eg: eng cat or en ca. <br>

An important point to mention here is that the algorithm strictly uses ISO 639-3 for language codes. This is the standard for the 3 letter language codes in the apertium-trunk. Some pairs in apertium still use 2-letter language codes (ISO 639-1), and for this purpose, when a bidix is parsed into an internally readable format, it's automatically changed to 3-letter codes. For this, the Wikipedia mapping (https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes) is used. For the lang-file-list it's possible to also provide 2-letter language codes (ISO 639-1) as mapping from 2-letter to 3-letter codes is done at the parsing step. However, for the config files described later it is necessary  to stick to the 3 letter codes. <br>

So the lang-file-list is a concatenation of multiple such blocks: <br>

> Path 1 <br>
>
> LeftLang code RightLang code for language pair 1 <br>
>
> Path 2<br>
>
> LeftLang code RightLang code for language pair 2<br>
>
> . <br>
>
> . <br>
>
> Path N <br>
>
> LeftLang code RightLang code for language pair N <br>

You can refer to "ApertiumRaw-langfile-list.txt" provided in the download as an example. <br>

The way to convert Apertium data to the internal format is by compiling using <code> g++ -o parse -std=c++17 BidixParsing.cpp -lstdc++fs </code> followed by <code>./parse path_to_lang-file-list </code> .  Similarly for RDF, it is expected that the RDF data is available in a CSV format, where there are spaces before and after the comma. The compilation command is <code> g++ -o parse -std=c++17 RDFCSVParsing.cpp -lstdc++fs </code> followed again by <code>./parse path_to_lang-file-list </code> <br>

The output (internal format) files are then available in the "Parsed" folder inside LangData. 

<h2> Usage Guidelines </h2>
After compiling using <code> g++ -o bidixgen -std=c++17 CLI.cpp -lstdc++fs </code>, the execution command is: <br>

<code> ./bidixgen [arguments] </code> <br>

<h3> Arguments - Quick Guide Option Tree</h3>
<ol>
    <li> <code> --expt_name="custom_name_here" </code> Required option, non-empty value must be specified. </li>
<li> Mode of execution (choose exactly one and follow the steps listed under it)
<ol type = 'A'>
<li> <code> -possible_translations </code> No value to be specified.
<ol type = 'a'>
<li> <code> --hyperparameter_file="path_to_hyperparameter_config_file" </code> [Optional] Used to provide a custom hyperparameter file, nly to be used in-case the defaults need to be tinkered with. </li>
<li> Generate translations for specific words or an entire language pair (must provide exactly one)
<ol type = 'i'>
<li> <code> --word_file="path_to_word_config_file" </code> If specific words are to be translated.
<li> <code> --lang_file="path_to_langpair_config_file" </code> If entire language pairs have to be translated.
</ol>
</ol>
</li>
<li> <code> -get_predictions</code> No value to be specified.
<ol type = 'a'>
<li> <code> --confidence=" "</code> [Optional] Takes decimal value in [0, 1]. Default value: 0.65
<li> <code>--folder_file="path_to_folder_config_file"</code> Necessary to specify.
</ol>
</li>
</ol>
</ol>


Only sequences of steps from the root to the leaf of the above option-tree form an accurate command. <br>
All file paths can be relative (to folder of execution) or absolute path along with it. How to create these files is described later. <br>
You can use the first letter of the attribute as a short form for all attributes, eg: <code> --e="custom_name_here" </code> for specifying the experiment name or <code>-g</code> for get_predictions <br>

Some examples:

<code> ./bidixgen --e=TestRun -p --h="../hp_config.txt" --l="../lang_config.txt"</code> - Generates possibilities in the TestRun folder <br>

<code> ./bidixgen --e=TestRun -g --f=../folder_config.txt </code> - Generates predictions in the TestRun folder from the possibilities generated in the previous command. <br>

<code>./bidixgen --e=TestRun -g --c=0.55 --f="../folder_config.txt" </code> - Generates predictions from the possibilities file generated in command 1 taking all translations with confidence score > 0.55 <br>

<h3> Detailed Explanation of Arguments </h3>

<h4>1. --expt_name </h4>

It is important to understand why this is asked. The output of each run produces or overwrites data in a folder named with the experiment name you input. This folder is present inside Results/ (also produced by the program if it doesn't already exist). So for example if you enter '2' then a folder with path Tool/Results/2/Analysis/ will contain the actions of the program. <br>
This essentially allows the user to run the program with separate config parameters and get results in separate folders to analyze what config parameters works best for them. <br>

<h4> 2. Execution Mode </h4>

There are 2 possible modes, <code> -possible_translations </code> and <code> -get_predictions </code>. <br>

Here, it is important to know that on running the main algorithm (-possible_translations), it produces a 'possibilities.txt' file in the target experiment folder. These are translations accompanied by their 'confidence' scores. Then, the user can execute <code>./bidixgen</code> with appropriate arguments again and convert the possibilities into predictions, produced in the 'predictions.txt' file. This is done to allow the user to play with different confidence thresholds and see which suits their needs best. 

<h5> 2A. -possible_translations </h5>

The program has some default hyperparameters as described later, and these can be changed by you as the user to see what works best for your language-pair and requirements. This can be done by adding <code> --hyperparameter_file="path_to_hyperparameter_config" </code>as an attribute. <br>

The hyperparameters that you want should be available in a file. Suppose it's called "hyperparameters.txt" and it's stored in the folder right outside Codes (where CLI.cpp runs from). You then enter "../hyperparameters.txt". More on how this hyperparameter file can be configured later. <br>

Finally, the program provides 2 options. Either generate translations for some words or for some entire language pairs.  <br>

Enter exactly one of these arguments (for words, language-pairs respectively) <br>

<code> --word_file="path_to_word_file"</code> <br>

or

<code> --lang_file="path_to_lang_file"</code> <br>

Provide the files just like the hyperparameters file above.  The config files required for the 2 options have to be formatted differently. More on that later. <br>
After this, the algorithm for generation will start running! It will provide some output like 'Loaded' within a minute which means that the input data has been read. You will then get outputs as different parts of your required output get processed (eg after each language-pair/word-translations are generated). It can take upto 10 minutes per language-pair generated normally.<br>

Once the program has terminated check the Results/ExperimentName/Analysis folder. It would have folders corresponding to the words/language-pairs you have produced translations for!

<h5> 2B. -get_predictions </h5>

As mentioned before, in get_predictions mode, the program converts an existing possibilites.txt file in possibly multiple folders (as defined in the folder_file attribute described soon) within an experiment. <br>

First you can optionally change the confidence score value from the default 0.65 by specifying <code>--confidence=0.6</code> or 0.8 (if you want to minimise false positives, or 0.5 (if you really want to be liberal) etc. Any value from [0, 1] is permissible. <br>

You must necessarily provide a folder_config file describing which folders this conversion from possibilities to predictions must take place. This is done by adding <code>--folder_file="path_to_folder_config_file"</code> <br>

This is because in the same experiment folder, multiple folders can exist for different language pairs, word translations etc. More on this later <br>

The program will now execute and generate the predictions.txt based on the possibilities.txt inside these folders. <br>
Example output: <br>

> doll done! <br>
> eng-cat done! <br>

That's it!

<h2> Config File Guidelines </h2>

There are 4 types of config files at present. a) hyperparameter file, b) language-pair file, c) word-file d) folder-file. The first 3 are for possible_translations mode and the last one for get_predictions. Samples for all of the 4 types are provided in the download: "hyperparameters.txt", "langconfigfast.txt", "wordconfig.txt", "folderconfig.txt". Throughout the config files, make sure you stick to the 3-letter language codes. <br>

As mentioned before, there are 2 main ways to generate new possibilities. One is a word-by-word approach where translations are found for a specified list of words. The other is a language pairs approach where results for multiple language-pairs can be generated (or as you will see, all possible predictions irrespective of language-pair). Both modes require a hyperparameters config file which is fed in earlier. Both modes have a separate type of config file described below.

<h3> Language-Pair Config file </h3>

A 'block' in a language-pair config file consists the follows: <br>
> Output Language Pair as "en es" or "NoFix integer" <br>
> Number of input language pairs, say M <br>
> SL<sub>1</sub> SL<sub>2</sub> SL<sub>3</sub> ... SL<sub>M</sub> <br>
> TL<sub>1</sub> TL<sub>2</sub> TL<sub>3</sub> ... TL<sub>M</sub> <br>

The first line contains the target language pair for which entries have to be generated. So for eg: "eng spa" means generating entries from english to spanish (and vice-versa). "eng-spa" will also become be the name of the folder in which results will be generated. Notably, it's also possible to not restrict the predicted translations' language pair by specifying the first line of the block as "NoFix 1" or "NoFix 2" or any other integer with "NoFix" (case-sensitive). In this case results will be produced in the "NoFix-1" or "NoFix-2" etc. folder. <br>
The second line contains the number of input language pairs, say N. This data will be used to produce predictions and the input data must be available in the LangData/Parsed folder with file names like "eng-spa.txt". The 3rd and 4th line together contain the description of these N language pairs. The ith string on both lines forms a language pair. So for eg if line 3 and 4 are: <br>

> eng eng fra <br>
> cat spa cat <br>

This means the input language pairs are eng-cat, eng-spa and fra-cat. Also, in this example the second line of the block must have 3. <br>

The first line of the language-pair config file consists of the number of blocks, say M. Description for M blocks follow in the following 4M lines. Example: <br>

> Number of blocks, M <br>
> 4 LINES describing Block 1 <br>
> 4 LINES describing Block 2 <br>
> . <br>
> . <br>
> 4 LINES describing Block M <br>

Examples are provided in "langconfigfast.txt" and "languageconfig.txt", specifically the latter reproduces the experiment of removing 1 language pair and using the other 10 as input. <br>

<h3> Word Config file </h3>
The brief format of the word config file is as follows: <br>

> Output File Name <br>
> Number of words to find translations for, say N <br>
> Word 1 <br>
> Word 2 <br>
> .
> .
> Word N<br>
> Number of input language pairs, say M<br>
> SL<sub>1</sub> SL<sub>2</sub> SL<sub>3</sub> ... SL<sub>M</sub> <br>
> TL<sub>1</sub> TL<sub>2</sub> TL<sub>3</sub> ... TL<sub>M</sub> <br>

The output file name is where the final output results will be produced. The 2nd line contains the number of words for which the user needs translations. Then, one word per line needs to be added. Finally, the input language pair description which is used for generation follows much as explained above.
<h3> Hyperparameter Config file </h3>
First, it is important to understand the different kind of hyperparameters that can be tuned:
<h4> Understanding sets of Hyperparameters </h4>
<ol>
	<li> <b> Transitive </b> (alias: transitive) - Whether the translations should be produced in a transitive way or not. Actually this takes a non-binary value, specifically from 0-2. 0 means to use the proper cycle-algorithm. 1 means to use transitivity within the biconnected component (atleast 1 cycle must pass through source-word, target-word). 2 means a proper transitive relation. If this is set to 1 or 2, only the Context Depth hyperparameter is applicable, which defines the depth to which transitive relations are to be considered.
    <li> <b> Context Depth </b> (alias: context_depth) - Maximum length of shortest path from source word to target word <br>
    Increasing this: a) Increases Run-Time b) Increases |Pred| and precision (performance) till a point, after that becomes useless/reduces performance. Recommended range: [2, 5]. Default: 3 if transitive closure, 4 otherwise. Allowed range: [1, 10] 
    <li> <b> Maximum Cycle Length </b> (alias: max_cycle_length) - Maximum length of cycle <br>
    Increasing this: a) Increases Run-Time b) Increases |Pred| and precision (performance) till a point, after that becomes redundant. Big dense cycles will have small denser subcycles. Recommended range [6, 9]. Default: 7. Allowed range: [4, 12]
    <li> <b> Cutoff for being 'Large Context' </b> (alias: large_cutoff) - Minimum degree of source word for it to be 'Large Context' <br>
    Increasing means more contexts become small contexts and have lesser required minimum cycle length. If set to 0, all contexts are large context. Recommended range: [3, 7], Default: 5. Allowed range: [0, 12]
    <li> <b> Minimum Cycle Length in Large Contexts </b> (alias: large_min_cyc_len) - Minimum length of cycle in Large Contexts i.e. Deg(Source) greater or equal Cutoff <br>
    Increasing this: a) Decreases |Pred| b) Could slightly increase precision but don't make it too high/too low c) Slightly Decreases Run-Time. Recommend range: [4, 6]. Default: 5. Allowed range: [0, 10]
    <li> <b> Minimum Cycle Length in Small Contexts </b> (alias: small_min_cyc_len) - Minimum length of cycle in Small Contexts <br>
    Increasing this: Same as above. Recommend range: [4, 5]. Default: 4. Allowed range: [0, 8]
    <li> <b> Confidence Multiplier if Target has degree > 2 </b> (alias: deg_gt2_multiplier) - If the target has degree more than 2, confidence score is multiplied by this value <br>
     Increasing this: a) Increases |Pred| b) Might decrease Precision. Recommended Range: [1, 1.5]. Default: 1.3. Allowed range: [0.5, 3.0]
    <li> <b> Confidence Threshold for predictions </b> (alias: conf_threshold) - Confidence Score cutoff for prediction to be be valid <br>
     Increasing this: a) Increases Precision b) Decreases |Pred|. Recommended Range: [0.5, 0.8]. Default: 0.65. Allowed range: [0.1, 1.0]


In practice, tuning 1), 2), 3) and 8) should be more than sufficient. Confidence can be tuned after generating the 'possibilities' file (which contains all possible translations with confidence > 0) by using the possibilities to prediction feature.

 <h4> Different Hyperparameter Sets for Different POS </h4>
 It is possible to define different sets of hyperparameters for different parts of speech. By default, properNouns and numerals have a transitive POS, whereas others use cycle density algorithm defaults as described above. You can define multiple sets of hyperparameters in your hyperparameters file and then provide a mapping from POS to the hyperparameter set number as described in the file format below.

<h4> How to format the hyperparameters file </h4>
Right now, the formatting requirements for the hyperparameters file is quite strict. It is white-space sensitive, so please adhere to the format carefully. <br>
The formal structure of the file is as follows: <br>

> *Hyperparameter Set* 0 <br>
> end<br>
> *Hyperparameter Set* 1<br>
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
> ENDOFFILE<br>

*Hyperparameter Set* as defined above is a group of lines of the following form: <br>

> transitive = 0<br>
> context_depth = 4<br>
> max_cycle_length = 9<br>
> large_cutoff = 5<br>
> large_min_cyc_len = 5<br>
> small_min_cyc_len = 4<br>
> deg_gt2_multiplier = 1.1<br>
> conf_threshold = 0.65<br>

The given aliases are both case sensitive and any change in spelling would make them unrecognizable for the program. Note that it is not important to add specific values for all hyperparameters. If the value for a specific hyperparameter is not mentioned, it's default value will be used. So for example, cross-checking with the defaults above, the above set differs only in max_cycle_length and deg_gt2_multiplier. So it is sufficient to only specify them without any changes to the actual output/meaning: <br>
> max_cycle_length = 9<br>
> deg_gt2_multiplier = 1.1<br>

A sample file ("hyperparameters.txt") is also provided to understand this paradigm. <br>

Note that the POS allowed have a fixed set of possible values and are case, white-space sensitive. These are: <br>

<code> noun, properNoun, verb, adverb, adjective, numeral, pronoun, preposition, punctuation, article, determiner, conjunction, particle </code><br>

Note that in the POS_To_Hyperparameter_Map the indexes to the hyperparameter set are 0 indexed, much like how the hyperparameter sets were labelled using 0-indexing in the format mentioned above. 

<h3> Folder Config File </h3>

This config file, required by the get_predictions mode, is the easiest to handle. It's input format is:

> Number of folders for converting possibilities to predictions, say N  <br>
>
> Folder_1_Name <br>
>
> Folder_2_Name <br>
>
> . <br>
>
> . <br>
>
> . <br>
>
> Folder_N_Name <br>

These folder names can be of language-pair data or word-translations data as they're both formatted similarly.

<hr> </hr>
In totality, it may be a bit overwhelming at once. Try to not play with the config files and use the ones provided in the install first. Using word-translations mode runs fast so use that initially. Then, start by making a new hyperparameter file and adding custom hyperparameters. Then customize the word-config / language-pair config and slowly it's easy to understand :)

<h3> Note on program outputs and issues </h3>

As many steps in the algorithm take a long time to run, intermediate output is often provided. Moreover, as the input format can be a little complex and easy to make mistakes in, efforts have been made to provide detailed error-handling, warning messages. In cases where feasible, the program just ignores a particular erroneous input line and continues, but else-where it may be forced to exit with an error status. Please read the error-message, and try to fix something in your config files by referring to the detailed README instructions above. It is wise to not deviate at all from the above mentioned formats as we might have not handled certain types of erroneous input, which could lead to weird output or long run-times. In-case the error seems in-explainable, please open an issue and for faster responses ask on the IRC or use the Apertium mailing-list. Any feedback is appreciated!
