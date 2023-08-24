#include <iostream>
#include <string>
#include <Eigen/Dense> 
#include <random>
#include <any>

//goal = create two classifiers, one that favours precision, one the favours recall. compact but effective. 

Eigen::MatrixXd generate_dataset(){
    //ip location (0 = australia, 1 = outside australia), typical ip location (0 = australia, 1 = outside australia),time of purchase, average time of purchase (0.0 - 23.59) , $$ spent, average $$ spent, 2FA enabled? (0 = no, 1 = yes), known retailer? (0 = no, 1 = yes), known-retailer/unknown ratio %, failed log in attempts? (0 = no, 1 = yes) 
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> on1(0,1);
    std::uniform_real_distribution<float> timeh(0,23.0);
    std::uniform_real_distribution<float> timemin(0,0.59);
    std::uniform_real_distribution<float> dollar_spent(1.0,10000.0);
    std::vector<std::vector<float>> b4matrix; 
    Eigen::MatrixXd my(100,10); 

    for (int rows = 0; rows < 100; rows++){

        std::vector<float> myVec;
        
        float ip_location = std::round(on1(gen));
        float typical_ip_location = on1(gen);
        float time_of_purchase = std::round((timeh(gen) + timemin(gen))*100.f)/100.f;
        float avrg_time_of_purchase = std::round((timeh(gen) + timemin(gen))*100.f)/100.f;
        float dollars_spent = dollar_spent(gen);
        float avrg_dollar_spent = dollar_spent(gen);
        float tofa_enabled = std::round(on1(gen));
        float known_retailer = std::round(on1(gen));
        float known_retailer_ratio = std::round(on1(gen));
        float failed_login_attempts = std::round(on1(gen));

        myVec.insert(myVec.begin(), {ip_location, typical_ip_location, time_of_purchase, avrg_time_of_purchase, dollars_spent, avrg_dollar_spent, tofa_enabled, known_retailer, known_retailer_ratio, failed_login_attempts});
        b4matrix.push_back(myVec);
    
    }

    for (int r = 0; r < b4matrix.size(); r++){
        for (int c = 0; c < b4matrix[0].size(); c++){
            my(r,c) = b4matrix[r][c];
        }
    }

    //std::cout << "my.row(0) = " << my.row(0) << std::endl;
    return my;

}

std::vector<float> feature_importance_score(){
    std::vector<float> fis = {1,0.5,2};
    return fis;
} 


Eigen::MatrixXd normalise_dataset(Eigen::MatrixXd data_set){
    Eigen::MatrixXd norm_d = data_set;
    for (int r = 0; r < data_set.rows(); r++){
        norm_d(r,2) /= 23.59;
        norm_d(r,3) /= 23.59;
        norm_d(r,4) /= 10000;
        norm_d(r,5) /= 10000;
    }

    //std::cout << "norm_d.row(0) = " << norm_d.row(0) << std::endl;

    return norm_d;

}


int main(){
    //{ip_location, typical_ip_location, time_of_purchase, avrg_time_of_purchase, dollars_spent, avrg_dollar_spent, tofa_enabled, known_retailer, known_retailer_ratio, failed_login_attempts}
    Eigen::MatrixXd data_set = generate_dataset();
    Eigen::MatrixXd normalised_data_set = normalise_dataset(data_set);
    std::vector<float> feature_importance_scre = feature_importance_score();
    std::vector<float> distance_scores;
    std::vector<float> absolute_differences;
        
    //each person is represented by a row, calculate a value that represents how much they deviate from their baseline activaty. 
    for (int r = 0; r < normalised_data_set.rows(); r++){
        Eigen::MatrixXd person = normalised_data_set.row(r);
        float score = (((normalised_data_set(r,0) - normalised_data_set(r,1)) * (normalised_data_set(r,0) - normalised_data_set(r,1))) * feature_importance_scre[2] + ((normalised_data_set(r,2) - normalised_data_set(r,3)) * (normalised_data_set(r,2) - normalised_data_set(r,3))) * feature_importance_scre[1] + ((normalised_data_set(r,4) - normalised_data_set(r,5)) * (normalised_data_set(r,4) - normalised_data_set(r,5))) * feature_importance_scre[0] + normalised_data_set(r,6) * 2 + ((normalised_data_set(r,7) - normalised_data_set(r,8)) * (normalised_data_set(7,0) - normalised_data_set(r,8))) * feature_importance_scre[0] + normalised_data_set(r,9) * 2 ) / normalised_data_set.cols(); // tells me the average deviation from what is expected over all 10 features. 
        distance_scores.push_back(score);
    }

    std::sort(distance_scores.begin(), distance_scores.end());

    //calculate median value.
    std::vector<float> median_score_corpus = distance_scores;
    median_score_corpus.erase(median_score_corpus.begin(),median_score_corpus.begin()+49);
    median_score_corpus.erase(median_score_corpus.begin()+2,median_score_corpus.end());
    float median = (median_score_corpus[0] + median_score_corpus[1])/2;
    

    for (auto val : distance_scores){
        std::cout << "score = " << val << std::endl;
    }

    
    std::cout << "\nrecall favouring (threshold = median)" << std::endl;
    std::cout << "-----------------------------------------------------\n" << std::endl;
    for (int x = 0; x < data_set.rows(); x++){
        if (distance_scores[x] >= median){
            std::cout << "fair chance person " << x << " is fraudulent" << std::endl;
            std::cout << "values: " << data_set.row(x) << "\n" << std::endl;
        }
    }

    
    //calculate the median of absolute differences.
    for (auto val : distance_scores){
        float absolute_diff = std::sqrt((val - median) * (val - median));
        absolute_differences.push_back(absolute_diff);
    }

    std::sort(absolute_differences.begin(), absolute_differences.end());
    absolute_differences.erase(absolute_differences.begin(),absolute_differences.begin()+49);
    absolute_differences.erase(absolute_differences.begin()+2,absolute_differences.end());
    float median_of_absolute_deviation = (absolute_differences[0] + absolute_differences[1])/2;

    std::cout << "median is: " << median << std::endl;
    std::cout << "median_of_absolute_deviation = " << median_of_absolute_deviation << "\n" << std::endl; //tells me the centre distance between the median and datapoints


    std::cout << "\n\nprecision favouring (theshold = 3.2+ median absolute deviations away)" << std::endl;
    std::cout << "------------------------------------------------------------------------------\n" << std::endl;
    
    //calculate the z score using median and median of absolute deviation. flag as anomaly if > 3.
    for (int dist_i = 0; dist_i < distance_scores.size(); dist_i++){
        float z_score = (distance_scores[dist_i] - median) / median_of_absolute_deviation;
        if (z_score >= 3.2f){
            std::cout << "high chance person " << dist_i << " is fraudulent" << std::endl;
            std::cout << "values: " << data_set.row(dist_i) << "\n" << std::endl;
        }
        

    }
    

   

    return 0;
}