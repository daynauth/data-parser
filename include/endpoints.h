struct {
    char * model;
    char * prompt;
    int steam;

    struct {
        int num_keep;
        int seed;
        int num_predict;
        double top_k;
        double top_p;
        double min_p;
        double tfs_z;
    } options;
} ollama;
