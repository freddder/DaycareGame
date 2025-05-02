function get_next_line(input)
	if dialogIndex ~= 0 and dialog[dialogIndex].quit ~= nil then
		dialogIndex = 0
		return nil
	end

	if dialogIndex ~= 0 and dialog[dialogIndex].choices ~= nil then
		dialogIndex = dialog[dialogIndex].choices[input].next
	else
		dialogIndex = dialogIndex + 1
	end

	if dialog[dialogIndex] == nil then
		dialogIndex = 1
		return nil
	end

	s = dialog[dialogIndex].text
	return s
end

function interact(input)
	s = get_next_line(input)
	return s
end