package main

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"math/rand"
	"net/http"
	"strings"
	"time"
)

const SentencesCount = 5
const RunesCount = 5

func main() {
	var sentences [SentencesCount]string

	for i := 0; i < SentencesCount; i++ {
		log.Printf("Get sentence #%d \n", i+1)
		sentences[i] = getSentence()
	}

	runes := getRandRunes(RunesCount)

	indexes, words := indexOfCharsInLastWord(sentences, runes)

	var runesToChar [RunesCount]string
	for i, r := range runes {
		runesToChar[i] = string(r)
	}

	for i, row := range indexes {
		log.Printf("Sentence: %s\n", sentences[i])
		log.Printf("Word: %s\n", words[i])
		log.Printf("Runes to find:  %+v\n", runesToChar)
		log.Printf("Finded indexes: %v\n\n", row)
	}
}

func indexOfCharsInLastWord(sentences [SentencesCount]string, chars []rune) ([][]int, []string) {
	indexes := make([][]int, len(sentences))
	lastWords := make([]string, len(sentences))

	for i, sentence := range sentences {
		words := strings.Fields(sentence)
		lastWord := words[len(words)-1]

		row := make([]int, len(chars))
		for j, char := range chars {
			row[j] = strings.Index(lastWord, string(char))
		}

		indexes[i] = row
		lastWords[i] = lastWord
	}

	return indexes, lastWords
}

type FishText struct {
	Status string
	Text   string
}

type TranslateText struct {
	DetectedLanguage struct {
		Language string
		Score    float64
	}
	Translations []struct {
		Text string
		To   string
	}
}

func getRandRunes(num int) []rune {
	rand.Seed(time.Now().UnixNano())

	alphabet := []rune("abcdefghijklmnopqrstuvwxyz")
	out := make([]rune, num)

	for i := 0; i < num; i++ {
		pos := rand.Intn(len(alphabet))
		out[i] = alphabet[pos]
	}

	return out
}

func getSentence() string {
	var ft FishText
	var tt []TranslateText

	fish, err := http.Get("https://fish-text.ru/get?type=sentence&number=1")
	if err != nil {
		log.Fatalln(err)
	}

	fishBody, err := ioutil.ReadAll(fish.Body)
	if err != nil {
		log.Fatalln(err)
	}

	err = json.Unmarshal(fishBody, &ft)
	if err != nil {
		log.Fatalln(err)
	}

	url := "https://microsoft-translator-text.p.rapidapi.com/translate?api-version=3.0&to=en&textType=plain&profanityAction=NoAction"
	payload := strings.NewReader("[{\"Text\": \"" + ft.Text + "\"}]")

	translate, err := http.NewRequest("POST", url, payload)
	if err != nil {
		log.Fatalln(err)
	}
	translate.Header.Add("content-type", "application/json")
	translate.Header.Add("x-rapidapi-key", "62cc005281msh249fea9dc911a96p1790bajsn4db15bdce8c0")
	translate.Header.Add("x-rapidapi-host", "microsoft-translator-text.p.rapidapi.com")

	r, err := http.DefaultClient.Do(translate)
	if err != nil {
		log.Fatalln(err)
	}
	defer r.Body.Close()

	translateBody, err := ioutil.ReadAll(r.Body)
	if err != nil {
		log.Fatalln(err)
	}

	err = json.Unmarshal(translateBody, &tt)
	if err != nil {
		log.Fatalln(err)
	}

	result := strings.ToLower(tt[0].Translations[0].Text)
	result = strings.Trim(result, " .!?")

	return result
}
