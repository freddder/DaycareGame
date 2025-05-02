dialogIndex = 0
dialog = {
	{ text = "hi" },
	{ text = "whatchu thinking?", choices = {
			{ text = "I dont think", next = 3 },
			{ text = "Whole lotta stuff", next = 4 }
		}
	},
	{ text = "impressive", quit = true },
	{ text = "ok" },
	{ text = "socrates" }
}