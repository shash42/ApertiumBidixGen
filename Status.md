<h4> Status of the Project </h4>

<h5> Approach </h5>

Currently, a more flexible version of the cycle density algorithm allowing different hyperparameters for different Parts of Speech is applied on precomputed Biconnected Components. The tunable hyperparameters provided are mostly a subset of those proposed in the original paper which were found to have a significant impact on results. <br>

Open areas of work include but are not limited to: <br>

<ol> 
    <li> Finding new types of constraints and hyperparameters to improve accuracy. Automatically guessing accurate values for existing hyperparameters based on the structure of the input graph.
    <li> Analyze subgraphs/contexts of wrongly generated translations to gain insights on possible improvements
    <li> Consider optimizing the idea of finding a maximum density cycle. Some possible ways are:
        <ol type=a>
            <li> Using Algebra, perhaps exploiting properties of <a href="https://www.codeproject.com/Articles/1158232/Enumerating-All-Cycles-in-an-Undirected-Graph)"> Fundamental Cycles and Cycle Basis</a>. This can especially be useful to speed-up the computation of cycle-density for a given cycle. 
            <li> Modifying <a href="https://users.ics.aalto.fi/gionis/goldberg-techreport.pdf">Goldberg's Maximum Density Subgraph</a> approach to find maximum density cycles. 
            <li> Use discrete optimization techniques like local search. 
            <li> Implement research ideas that allow the required amount of flexibility in terms of validating certain substructures (as encoded in hyperparameters currently)
        </ol>
	This will be of limited benefit though as there are diminishing returns to increasing the maximum length of cycles processed. This is because words get farther away, and sub-cycles of length 6 or 7 within a larger cycle are likely to be more dense anyway. Moreover, the current implementation is fast enough (a matter of minutes) because practically, generating language-pairs is more or less a one-time task and no dynamic real-time queries are being answered.
    <li> Consider analyzing substructures of the multilingual graph of translations to find new insights
    <li> Consider using probablities. An interesting read is <a href="https://www.aclweb.org/anthology/P09-1030.pdf"> Massive multilingual inference using SenseUniformPaths </a> Another similar direction could be applying <a href="https://en.wikipedia.org/wiki/Graphical_model"> Probabilistic Graphical Models </a>
    <li> The possibility for a complete revamp in approach is always open :) Perhaps using a neural model like Graph Neural Networks
</ol>


<h5> Implementation </h5>

A modular pipeline for predicting translations and facilitating easy experimentation with hyperparameters has been implemented. It can be described as follows: <br>

<ol>
    <li> Input: Parsers for Apertium and RDF Data to a simple internal TSV format. These are separate from the main pipeline described below as parsing is a one-time operation per bilingual dictionary  </li>
    <li> Graph class: Instead of using a Graph class from external libraries like Boost, a custom class was made specific to this project. This also allows it to be modified at will in the future.
    <li> Dense Cycle finding: For a given length upper-bound k, there are O(n<sup>k</sup>) cycles in the subgraph. This is a bottleneck in our pipeline even though linguistic graphs are not dense. The current code precomputes biconnected components and uses a stack-based depth-first-search approach for cycle finding on each biconnected component independently. It breaks as soon as a pruning constraint can no longer be satisfied.
    <li> Metrics class: To allow experimenting with new constraints for cycle-pruning and new scoring methods, a separate metrics class is stored for each cycle that satisfies the constraints for a given pair of words. This makes extracting statistics from the graph and processing them to generate confidence scores orthogonal allowing an easier setup for experimentation and improvement.
    <li> Callers: The process of generating predictions is to first run the algorithm such that it generates a list of all possible (score > 0) translations. At this stage, instances with different configurations of the earlier pipeline can be called for different POS. Seeing the results of the generated possibilities file, the user can then try different confidence score cutoffs to see what brings them the best results. 
    <li> Interface: A command line interface has been provided to the user, where along with arguments the user has to provide config files for different aspects: which language-pair/words to generate translations for, what input data to use, what hyperparameters to use, which folders to generate predictions using existing possibilities files etc. A lot of input validation has already been implemented, but the tool probably isn't foolproof if someone deliberately tries to hack it to behave weirdly. This should not be a major issue as it's an individually downloaded tool, not a public facing platform.
</ol>


Future possibilities: <br>

<ol> 
    <li> Allowing an arbitrary amount of information to be stored in the WordData class, not just POS, without losing speed. This was tried by using a map or unordered_map but it considerably slows down the algorithm. It was hence commented out and can be re-added in the future.
    <li> A smoother installation sequence.
    <li> Better usage messages when errors come up. Building perhaps a regex-based evaluation to ensure inputs match constraints. Making it more robust to invalid input in general.
    <li> Currently, 2 lemmas with the same written representation (colloquially spelling) belonging to the same PartOfSpeech are not treated differently, i.e. they belong to the same node. An example is the verb 'lie' as in 'lie on the bed' and 'lie' as in 'stop telling lies'. These are rare outliers, and have limited effect on overall usability. Since the idea itself is to work without semantic information, it's unlikely these can be distinguished. But it's worth keeping this in mind.
</ol>



<h5> Features </h5>

A detailed list of the currently available possible options is provided in the Tool folder README. <br>

<ol>
    <li> Automatically choosing input language pairs based on target output language pair.
    <li> Allow showing detailed information on why a particular translation was generated (no. of cycles, average cycle density, best cycle etc.)
    <li> A GUI for easy management of config files, results, input files, downloads for apertium bidixes etc.   
</ol>


<h5> Apertium Specific </h5>

More than 90% of Apertium data can be parsed with the current parser. Apertium format output can be provided using the --bidix-output option with the confidence score in comments and a marker that the BidixGen algorithm produced it like &lt;e="BidixGen" c="score:0.76">.  <br>

However, morphological information is currently not taken inside the pipeline. We arrived at the conclusion that propagating morphological information throughout the pipeline will multiply the number of nodes by a significant factor, reduce graph density (thus affecting accuracy) and also make the algorithm slower. The source and target words of any predicted translation would always exist in their language's monodix as the source data itself is Apertium. Thus, using the respective monodix entries, all required morphological transfers for the bidix can be generated through a separate script. [This has already been implemented](https://github.com/apertium/apertium-scn-spa/tree/master/dev/bidix_templates) by spectie (Francis Tyers) and khannatanmai (Tanmai Khanna) for the unreleased scn-spa pair, so it's definitely feasible. Some tweaks in the algorithm might be required (like propagating &lt;g/> tag positions) to make full use of their script. <br>

Future possibilities: <br>

<ol>
    <li> Get this added to all main Apertium Guide-Wikis and perhaps even Apertium's repositories so that all new language pair developers use it!
    <li> Integrate the "Templates" script with monolingual dictionaries into the current pipeline itself.
    <li> Parsing of bidix entries with the &lt;re&gt; tag.
    <li> The &lt;g/&gt; tag that marks morphology inflection positions is currently ignored/not handled. 
    <li> When there are multiple &lt;s&gt; tags with POS-like info to use, Eg: &lt;e&gt;&lt;p&gt;&lt;l&gt;навошта&lt;s n="adv"/>&lt;/l&gt;&lt;r&gt;зачем&lt;s n="adv"/>&lt;s n="itg"/>&lt;/r&gt;&lt;/p&gt;&lt;/e&gt;, the first &lt;s&gt; tag value is taken into consideration. The same happens when multiple tags are placed in a '_' separated manner. This heuristic can be improved.
    <li> Currently POS is extracted from diverse apertium tags using a modified version of the <a href="https://github.com/sid-unizar/apertium-lexinfo-mapping">apertium-lexinfo-mapping</a>. It's a static file in the Tool folder which won't get updated as Apertium grows. It would be nice if it is updated as new tags are introduced by language pair developers in Apertium.
</ol>



<h5> Testing and Evaluation </h5>

Initial tests were run by taking 11 RDF language-pairs across 6 'well-connected' languages (english, french, esperanto, occitan, catalan, spanish) using 10 of them as input and trying to generate the 11th from scratch. The generated predictions were compared using the RDF data of the language-pair as a 'gold standard'. This was not exactly true because while the source data entries were 'correct', they were not exhaustive. To handle this, Precision and Recall were broken down into 4 categories. a) Both words of the predicted translation exist in the RDF language-pair b) only the SL word existed c) only the TL word existed and lastly d) neither word existed. A detailed description of this can be found [here](https://hackmd.io/@o7NQTGl-SCCmH18WdRRuzA/r1A40BXyw).<br>

The first category was used as a heuristic to evaluate the performance of the algorithm, because words being missing in the RDF language pair were clearly a case it's incompleteness, as these words made into the graph using some other language pair in the first place! Multiple hyperparameter settings were tried, and the optimal one finally generated outputs in 2-3 minutes per language pair with >90% Both-Vertex-Matched-Precision and >30% Both-Vertex-Matched-Recall across all important POS categories (Nouns, Verbs, Proper Nouns, Adjectives, Adverbs). <br>

Both these numbers are actually strict minimum bounds, with the actual value being higher due to the incompleteness of the "golden standard" data. This was confirmed through human evaluations. Random subsets of 150 predicted translations generated using a 'liberal' hyperparameters setting, that weren't in the source data (and hence not counted as 'correct'), were also evaluated by multiple volunteers. No serious issues were brought up during these evaluations, with a majority of these extra translations being correct. This confirms that the algorithm is producing useful results! <br>

<ol> 
 <li> Experiment using bilingual dictionaries from the Apertium Staging and Nursery. See which language pairs have good quality data.re
 <li> Correspond with language pair developers who initially use the tool to generate entries, and incorporate feedback.
 <li> Using the human evaluations procured, analyze which type of entries are going wrong and why they are generated.
 <li> Maintain the tool and as and when software engg. type issues / bugs get identified, fix them.
</ol>

<h5> Possible Future Applications of my work in GSoC'20 </h5>

<ol>
    <li> The BidixParsing code written for this project seems to be more effective than the one used by the RDF project. Convene on this and use the parser on this repository to enrich RDF data if that's true.
    <li> During the course of the project, non-trivial issues were identified in RDF linguistic data thus benefitting the RDF project. Similarly, a comprehensive analysis of Apertium bilingual dictionaries was taken up which brought up interesting insights. More such findings are bound to happen when working with the useful graph abstraction of multilingual data.
    <li> It is insightful to think about how word-sense gets encoded by using multilingual data. Taking two words u, v belonging to the same language, the algorithm's confidence score for their edge reflects their 'synonymity', i.e. whether they share a semantic sense. This can be an approach to finding 'word similarity'.
    <li> Explore applications of the project pipeline in relationship inference beyond linguistic data. Formally, linguistic data with information from k languages forms a k-partite graph. More generally, the algorithm performs relationship inference in this k-partite graph. It's likely the idea can be applied in multiple other domains like<a href="https://en.wikipedia.org/wiki/Semantic_similarity"> semantic similarity</a>.
</ol>